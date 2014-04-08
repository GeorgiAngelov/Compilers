#include "env.h"
#include "ast.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

static int get_symbol_value(const char*);
static void print_env_type(int, Type*, int);
static void insert_type_copy_deep(int, Type*, GHashTable*);
static void insert_env_copy_deep(int, Env*, GHashTable*);
static void insert_stmts_copy_deep(int, GList*, GHashTable*);
static void env_destroy_helper(int key, void* data, void (*destroy)(void*));

/*** Symbols. ***/

enum {
      SYMBOL_INVALID,
      SYMBOL_FIELD,
      SYMBOL_FUN,
      SYMBOL_TYPENAME,
      SYMBOL_VAR,
};

int symbol_is_valid(Symbol symbol) {
      return symbol.class == SYMBOL_FIELD
            || symbol.class == SYMBOL_FUN
            || symbol.class == SYMBOL_TYPENAME
            || symbol.class == SYMBOL_VAR;
}

int symbol_is_storable(Symbol symbol) {
      return symbol_is_valid(symbol)
            && symbol.class != SYMBOL_FIELD;
}

static int get_symbol_value(const char* id) {
      return (int)g_quark_from_string(id);
}

Symbol symbol_field(const char* id) {
      Symbol symbol;
      symbol.class = SYMBOL_FIELD;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_fun(const char* id) {
      Symbol symbol;
      symbol.class = SYMBOL_FUN;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_typename(const char* id) {
      Symbol symbol;
      symbol.class = SYMBOL_TYPENAME;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_var(const char* id) {
      Symbol symbol;
      symbol.class = SYMBOL_VAR;
      symbol.value = get_symbol_value(id);
      return symbol;
}

Symbol symbol_fun_return(void) {
      Symbol symbol;
      symbol.class = SYMBOL_VAR;
      symbol.value = get_symbol_value("$return");
      return symbol;
}

const char* symbol_to_str(Symbol symbol) {
      assert(symbol_is_valid(symbol));
      return (const char*) g_quark_to_string(symbol.value);
}

int symbol_is_field(Symbol symbol) {
      return symbol.class == SYMBOL_FIELD;
}

int symbol_is_fun(Symbol symbol) {
      return symbol.class == SYMBOL_FUN;
}

int symbol_is_typename(Symbol symbol) {
      return symbol.class == SYMBOL_TYPENAME;
}

int symbol_is_var(Symbol symbol) {
      return symbol.class == SYMBOL_VAR;
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
      switch (symbol.class) {
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
                  printf("(sym-var %s)", symbol_to_str(symbol));
                  break;
            default:
                  printf("(sym-invalid)");
                  break;
      }
}

/*** Environments. ***/

Env* env_new(void) {
      Env* this = calloc(1, sizeof(Env));
      assert(this);
      this->fun_defs = g_hash_table_new(NULL, NULL);
      this->fun_envs = g_hash_table_new(NULL, NULL);
      this->fun_types = g_hash_table_new(NULL, NULL);
      this->typename_types = g_hash_table_new(NULL, NULL);
      this->var_types = g_hash_table_new(NULL, NULL);
      return this;
}

Type* env_lookup(Env* env, Symbol symbol) {
      assert(env);
      assert(env->fun_types);
      assert(env->typename_types);
      assert(env->var_types);

      Type* type = NULL;
      switch (symbol.class) {
            case SYMBOL_FUN:
                  type = g_hash_table_lookup(env->fun_types, GINT_TO_POINTER(symbol.value));
                  break;
            case SYMBOL_TYPENAME:
                  type = g_hash_table_lookup(env->typename_types, GINT_TO_POINTER(symbol.value));
                  break;
            case SYMBOL_VAR:
                  type = g_hash_table_lookup(env->var_types, GINT_TO_POINTER(symbol.value));
                  break;
      }

      return type;
}

Env* env_lookup_fun_env(Env* env, Symbol symbol) {
      assert(env);
      assert(env->fun_envs);
      assert(symbol_is_fun(symbol));

      return g_hash_table_lookup(env->fun_envs, GINT_TO_POINTER(symbol.value));
}

GList* env_lookup_fun_def(Env* env, Symbol symbol) {
      assert(env);
      assert(env->fun_defs);
      assert(symbol_is_fun(symbol));

      return g_hash_table_lookup(env->fun_defs, GINT_TO_POINTER(symbol.value));
}

void env_insert(Env* env, Symbol symbol, Type* type) {
      assert(env);
      assert(env->fun_types);
      assert(env->typename_types);
      assert(env->var_types);
      assert(symbol_is_storable(symbol));
      switch (symbol.class) {
            case SYMBOL_FUN:
                  g_hash_table_insert(env->fun_types, GINT_TO_POINTER(symbol.value), type_copy_deep(type));
                  break;
            case SYMBOL_TYPENAME:
                  g_hash_table_insert(env->typename_types, GINT_TO_POINTER(symbol.value), type_copy_deep(type));
                  break;
            case SYMBOL_VAR:
                  g_hash_table_insert(env->var_types, GINT_TO_POINTER(symbol.value), type_copy_deep(type));
                  break;
      }
}

void env_insert_fun(Env* env, Symbol symbol, Type* type, Env* fun_env, GList* stmts) {
      assert(env);
      assert(symbol_is_fun(symbol));
      assert(env->fun_types);
      assert(env->fun_envs);
      assert(env->fun_defs);
      assert(type);

      g_hash_table_insert(env->fun_types, GINT_TO_POINTER(symbol.value), type_copy_deep(type));
      g_hash_table_insert(env->fun_envs, GINT_TO_POINTER(symbol.value), env_copy_deep(fun_env));
      g_hash_table_insert(env->fun_defs, GINT_TO_POINTER(symbol.value), stmts_copy_deep(stmts));
}

Env* env_union(Env* left, Env* right) {
      Env* env = env_new();

      if (left) {
            g_hash_table_foreach(left->fun_defs, (GHFunc)insert_stmts_copy_deep, env->fun_defs);
            g_hash_table_foreach(left->fun_envs, (GHFunc)insert_env_copy_deep, env->fun_envs);
            g_hash_table_foreach(left->fun_types, (GHFunc)insert_type_copy_deep, env->fun_types);
            g_hash_table_foreach(left->typename_types, (GHFunc)insert_type_copy_deep, env->typename_types);
            g_hash_table_foreach(left->var_types, (GHFunc)insert_type_copy_deep, env->var_types);
      }

      if (right) {
            g_hash_table_foreach(right->fun_defs, (GHFunc)insert_stmts_copy_deep, env->fun_defs);
            g_hash_table_foreach(right->fun_envs, (GHFunc)insert_env_copy_deep, env->fun_envs);
            g_hash_table_foreach(right->fun_types, (GHFunc)insert_type_copy_deep, env->fun_types);
            g_hash_table_foreach(right->typename_types, (GHFunc)insert_type_copy_deep, env->typename_types);
            g_hash_table_foreach(right->var_types, (GHFunc)insert_type_copy_deep, env->var_types);
      }

      return env;
}

int env_contains(Env* env, Symbol symbol) {
      assert(env);
      assert(symbol_is_storable(symbol));
      switch (symbol.class) {
            case SYMBOL_FUN:
                  return g_hash_table_lookup(env->fun_types, GINT_TO_POINTER(symbol.value)) != NULL;
            case SYMBOL_TYPENAME:
                  return g_hash_table_lookup(env->typename_types, GINT_TO_POINTER(symbol.value)) != NULL;
            case SYMBOL_VAR:
                  return g_hash_table_lookup(env->var_types, GINT_TO_POINTER(symbol.value)) != NULL;
      }
      return 0;
}

static void print_env_type(int symbol_value, Type* type, int symbol_class) {
      assert(type);
      Symbol symbol;
      symbol.class = symbol_class;
      symbol.value = symbol_value;

      switch (symbol_class) {
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
      type_print(type);
      puts("");
}

void env_print(Env* env) {
      assert(env);
      assert(env->fun_types);
      assert(env->typename_types);
      assert(env->var_types);
      g_hash_table_foreach(env->fun_types, (GHFunc)print_env_type, GINT_TO_POINTER(SYMBOL_FUN));
      g_hash_table_foreach(env->typename_types, (GHFunc)print_env_type, GINT_TO_POINTER(SYMBOL_TYPENAME));
      g_hash_table_foreach(env->var_types, (GHFunc)print_env_type, GINT_TO_POINTER(SYMBOL_VAR));
}

static void insert_type_copy_deep(int key, Type* type, GHashTable* ht) {
      assert(ht);
      g_hash_table_insert(ht, GINT_TO_POINTER(key), type_copy_deep(type));
}

static void insert_env_copy_deep(int key, Env* fenv, GHashTable* ht) {
      assert(ht);
      g_hash_table_insert(ht, GINT_TO_POINTER(key), env_copy_deep(fenv));
}

static void insert_stmts_copy_deep(int key, GList* stmts, GHashTable* ht) {
      assert(ht);
      g_hash_table_insert(ht, GINT_TO_POINTER(key), stmts_copy_deep(stmts));
}

Env* env_copy_deep(Env* env) {
      if (!env) return NULL;

      assert(env->fun_defs);
      assert(env->fun_envs);
      assert(env->fun_types);
      assert(env->typename_types);
      assert(env->var_types);

      return env_union(env, NULL);
}

static void env_destroy_helper(int key, void* data, void (*destroy)(void*)) {
      assert(destroy);
      destroy(data);
}

void env_destroy(Env* env) {
      if (!env) return;

      assert(env->fun_defs);
      assert(env->fun_envs);
      assert(env->fun_types);
      assert(env->typename_types);
      assert(env->var_types);

      g_hash_table_foreach(env->fun_defs, (GHFunc)env_destroy_helper, stmts_destroy);
      g_hash_table_destroy(env->fun_defs);

      g_hash_table_foreach(env->fun_envs, (GHFunc)env_destroy_helper, env_destroy);
      g_hash_table_destroy(env->fun_envs);

      g_hash_table_foreach(env->fun_types, (GHFunc)env_destroy_helper, type_destroy);
      g_hash_table_destroy(env->fun_types);

      g_hash_table_foreach(env->typename_types, (GHFunc)env_destroy_helper, type_destroy);
      g_hash_table_destroy(env->typename_types);

      g_hash_table_foreach(env->var_types, (GHFunc)env_destroy_helper, type_destroy);
      g_hash_table_destroy(env->var_types);

      free(env);
}

