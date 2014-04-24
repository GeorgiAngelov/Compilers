#include "env.h"
#include "ast.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

static struct record record(Type* type, Env* fun_env, GList* fun_body);
static struct record* record_copy_deep(const struct record* rec);
static void record_destroy(struct record* rec);
static void record_print(struct record* rec);

static int get_symbol_value(const char*);
static void print_entry(int, struct record*, int);

static void insert_record_copy_deep(int key, const struct record* rec, GHashTable* ht);

// This is the information stored in the environment for every symbol.
struct record {
      Type* type;
      Env* fun_env;
      GList* fun_body;
};

static struct record record(Type* type, Env* fun_env, GList* fun_body) {
      struct record me;
      me.type = type;
      me.fun_env = fun_env;
      me.fun_body = fun_body;
      return me;
}

static struct record* record_copy_deep(const struct record* rec) {
      assert(rec);
      struct record* copy = (struct record*)calloc(1, sizeof(struct record));
      assert(copy);

      *copy = record(
            type_copy_deep(rec->type), 
            env_copy_deep(rec->fun_env),
            stmts_copy_deep(rec->fun_body));
      return copy;
}

static void record_destroy(struct record* rec) {
      if (!rec) return;

      type_destroy(rec->type);
      env_destroy(rec->fun_env);
      stmts_destroy(rec->fun_body);

      free(rec);
}

enum {
      SYMBOL_INVALID = 256,
      SYMBOL_FIELD,
      SYMBOL_FUN,
      SYMBOL_TYPENAME,
      SYMBOL_VAR,
      SYMBOL_PSEUDO,
};

int symbol_is_valid(Symbol symbol) {
      return symbol.kind == SYMBOL_FIELD
            || symbol.kind == SYMBOL_FUN
            || symbol.kind == SYMBOL_TYPENAME
            || symbol.kind == SYMBOL_VAR
            || symbol.kind == SYMBOL_PSEUDO;
}

int symbol_is_storable(Symbol symbol) {
      return symbol_is_valid(symbol)
            && symbol.kind != SYMBOL_FIELD;
}

static int get_symbol_value(const char* id) {
      return (int)g_quark_from_string(id);
}

Symbol symbol_field(const char* id) {
      Symbol symbol;
      symbol.kind = SYMBOL_FIELD;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_fun(const char* id) {
      Symbol symbol;
      symbol.kind = SYMBOL_FUN;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_typename(const char* id) {
      Symbol symbol;
      symbol.kind = SYMBOL_TYPENAME;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_var(const char* id) {
      Symbol symbol;
      symbol.kind = SYMBOL_VAR;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_fun_return(void) {
      Symbol symbol;
      symbol.kind = SYMBOL_PSEUDO;
      symbol.value = get_symbol_value("$return");
      return symbol;
}

const char* symbol_to_str(Symbol symbol) {
      assert(symbol_is_valid(symbol));
      return (const char*) g_quark_to_string(symbol.value);
}

int symbol_is_field(Symbol symbol) {
      return symbol.kind == SYMBOL_FIELD;
}

int symbol_is_fun(Symbol symbol) {
      return symbol.kind == SYMBOL_FUN;
}

int symbol_is_typename(Symbol symbol) {
      return symbol.kind == SYMBOL_TYPENAME;
}

int symbol_is_var(Symbol symbol) {
      return symbol.kind == SYMBOL_VAR;
}

int symbol_is_pseudo(Symbol symbol) {
      return symbol.kind == SYMBOL_PSEUDO;
}

int symbol_equal(Symbol left, Symbol right) {
      return left.value == right.value;
}

int symbol_cmp(Symbol left, Symbol right) {
      if (left.value > right.value) return 1;
      if (left.value < right.value) return -1;
      return 0;
}

void symbol_print(Symbol symbol) {
      switch (symbol.kind) {
            case SYMBOL_FIELD:
                  printf("(sym-field %s)", symbol_to_str(symbol));
                  break;
            case SYMBOL_FUN:
                  printf("(sym-fun %s)", symbol_to_str(symbol));
                  break;
            case SYMBOL_TYPENAME:
                  printf("(sym-typename %s)", symbol_to_str(symbol));
                  break;
            case SYMBOL_VAR:
            case SYMBOL_PSEUDO:
                  printf("(sym-var %s)", symbol_to_str(symbol));
                  break;
            default:
                  printf("(sym-invalid)");
                  break;
      }
}

Env* env_new(void) {
      Env* me = (Env*)calloc(1, sizeof(Env));
      assert(me);
      me->funs = g_hash_table_new(NULL, NULL);
      me->typenames = g_hash_table_new(NULL, NULL);
      me->vars = g_hash_table_new(NULL, NULL);
      return me;
}

Type* env_lookup(Env* env, Symbol symbol) {
      assert(env);
      assert(env->funs);
      assert(env->typenames);
      assert(env->vars);

      struct record* rec = NULL;
      switch (symbol.kind) {
            case SYMBOL_FUN:
                  rec = (struct record*)g_hash_table_lookup(env->funs, GINT_TO_POINTER(symbol.value));
                  break;
            case SYMBOL_TYPENAME:
                  rec = (struct record*)g_hash_table_lookup(env->typenames, GINT_TO_POINTER(symbol.value));
                  break;
            case SYMBOL_VAR:
            case SYMBOL_PSEUDO:
                  rec = (struct record*)g_hash_table_lookup(env->vars, GINT_TO_POINTER(symbol.value));
                  break;
      }

      if (!rec) return NULL;
      return rec->type;
}

Env* env_lookup_fun_env(Env* env, Symbol symbol) {
      assert(env);
      assert(env->funs);
      assert(symbol_is_fun(symbol));

      struct record* rec = (struct record*)g_hash_table_lookup(env->funs, GINT_TO_POINTER(symbol.value));

      if (!rec) return NULL;
      return rec->fun_env;
}

GList* env_lookup_fun_def(Env* env, Symbol symbol) {
      assert(env);
      assert(env->funs);
      assert(symbol_is_fun(symbol));

      struct record* rec = (struct record*)g_hash_table_lookup(env->funs, GINT_TO_POINTER(symbol.value));

      if (!rec) return NULL;
      return rec->fun_body;
}

void env_insert(Env* env, Symbol symbol, Type* type) {
      assert(env);
      assert(env->funs);
      assert(env->typenames);
      assert(env->vars);
      assert(symbol_is_storable(symbol));

      GHashTable* ht = NULL;
      struct record rec = record(type, NULL, NULL);

      switch (symbol.kind) {
            case SYMBOL_FUN:
                  ht = env->funs;
                  break;
            case SYMBOL_TYPENAME:
                  ht = env->typenames;
                  break;
            case SYMBOL_PSEUDO:
                  ht = env->vars;
                  break;
            case SYMBOL_VAR: {
                  ht = env->vars;
                  break;
            }
      }

      assert(ht);
      insert_record_copy_deep(symbol.value, &rec, ht);
}

void env_insert_fun(Env* env, Symbol symbol, Type* type, Env* fun_env, GList* stmts) {
      assert(env);
      assert(symbol_is_fun(symbol));
      assert(env->funs);
      assert(type);

      struct record rec = record(type, fun_env, stmts);
      insert_record_copy_deep(symbol.value, &rec, env->funs);
}

Env* env_union(Env* left, Env* right) {
      Env* env = env_new();

      if (left) {
            g_hash_table_foreach(left->funs, (GHFunc)insert_record_copy_deep, env->funs);
            g_hash_table_foreach(left->typenames, (GHFunc)insert_record_copy_deep, env->typenames);
            g_hash_table_foreach(left->vars, (GHFunc)insert_record_copy_deep, env->vars);
      }

      if (right) {
            g_hash_table_foreach(right->funs, (GHFunc)insert_record_copy_deep, env->funs);
            g_hash_table_foreach(right->typenames, (GHFunc)insert_record_copy_deep, env->typenames);
            g_hash_table_foreach(right->vars, (GHFunc)insert_record_copy_deep, env->vars);
      }

      return env;
}

int env_contains(Env* env, Symbol symbol) {
      assert(env);
      assert(symbol_is_storable(symbol));
      switch (symbol.kind) {
            case SYMBOL_FUN:
                  return g_hash_table_lookup(env->funs, GINT_TO_POINTER(symbol.value)) != NULL;
            case SYMBOL_TYPENAME:
                  return g_hash_table_lookup(env->typenames, GINT_TO_POINTER(symbol.value)) != NULL;
            case SYMBOL_VAR:
                  return g_hash_table_lookup(env->vars, GINT_TO_POINTER(symbol.value)) != NULL;
      }
      return 0;
}

static void print_entry(int symbol_value, struct record* rec, int symbol_kind) {
      assert(rec);
      Symbol symbol;
      symbol.kind = symbol_kind;
      symbol.value = symbol_value;

      switch (symbol_kind) {
            case SYMBOL_FUN:
                  printf("fun(%s) |-> ", symbol_to_str(symbol));
                  break;
            case SYMBOL_TYPENAME:
                  printf("type(%s) |-> ", symbol_to_str(symbol));
                  break;
            case SYMBOL_VAR:
                  printf("var(%s) |-> ", symbol_to_str(symbol));
                  break;
      }
      record_print(rec);
      puts("");
}

static void record_print(struct record* rec) {
      assert(rec);
      printf("(");
      type_print_pretty(rec->type);
      printf(", ");
      if (rec->fun_env) printf("<fun_env>");
      else printf("NULL");
      printf(", ");
      if (rec->fun_body) printf("<fun_body>");
      else printf("NULL");
      printf(")");
}

void env_print(Env* env) {
      assert(env);
      assert(env->funs);
      assert(env->typenames);
      assert(env->vars);
      g_hash_table_foreach(env->funs, (GHFunc)print_entry, GINT_TO_POINTER(SYMBOL_FUN));
      g_hash_table_foreach(env->typenames, (GHFunc)print_entry, GINT_TO_POINTER(SYMBOL_TYPENAME));
      g_hash_table_foreach(env->vars, (GHFunc)print_entry, GINT_TO_POINTER(SYMBOL_VAR));
}

static void insert_record_copy_deep(int key, const struct record* rec, GHashTable* ht) {
      assert(ht);
      g_hash_table_insert(ht, GINT_TO_POINTER(key), record_copy_deep(rec));
}

Env* env_copy_deep(Env* env) {
      if (!env) return NULL;

      assert(env->funs);
      assert(env->typenames);
      assert(env->vars);

      return env_union(env, NULL);
}

static void env_destroy_helper(int key, void* data, void (*destroy)(void*)) {
      assert(destroy);
      destroy(data);
}

void env_destroy(Env* env) {
      if (!env) return;

      assert(env->funs);
      assert(env->typenames);
      assert(env->vars);

      g_hash_table_foreach(env->funs, (GHFunc)env_destroy_helper, (void*)record_destroy);
      g_hash_table_destroy(env->funs);

      g_hash_table_foreach(env->typenames, (GHFunc)env_destroy_helper, (void*)record_destroy);
      g_hash_table_destroy(env->typenames);

      g_hash_table_foreach(env->vars, (GHFunc)env_destroy_helper, (void*)record_destroy);
      g_hash_table_destroy(env->vars);

      free(env);
}

