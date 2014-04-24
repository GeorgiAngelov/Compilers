#include "type.h"
#include "env.h"
#include "ast.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

static Type* type_new(Type);
static int decl_cmp(const struct decl*, const struct decl*);
static int decl_equal(const struct decl*, const struct decl*);
static int decls_equal(GList* left, GList* right);
static void typed_ids_print(GList* decls);
static void typed_id_print(const struct decl* decl);
static void decl_print_pretty(const struct decl* tid);

/*** Constructors. ***/

Type type(int kind) {
      Type t;
      t.kind = kind;
      return t;
}

static Type* type_new(Type t) {
      Type* copy = (Type*)calloc(1, sizeof(Type));
      assert(copy);

      *copy = t;
      return copy;
}

Type* type_int_new(void) {
      Type t;
      t.kind = TYPE_INT;
      return type_new(t);
}
Type* type_bool_new(void) {
      Type t;
      t.kind = TYPE_BOOL;
      return type_new(t);
}
Type* type_nil_new(void) {
      Type t;
      t.kind = TYPE_NIL;
      return type_new(t);
}
Type* type_void_new(void) {
      Type t;
      t.kind = TYPE_VOID;
      return type_new(t);
}
Type* type_id_new(Symbol id) {
      Type t;
      t.kind = TYPE_ID;
      t.id = id;
      return type_new(t);
}
Type* type_struct_new(GList* fields) {
      Type t;
      t.kind = TYPE_STRUCT;
      t.fields = g_list_sort(fields, (GCompareFunc)decl_cmp);
      return type_new(t);
}
Type* type_array_new(Type* eles) {
      Type t;
      t.kind = TYPE_ARRAY;
      t.eles = eles;
      return type_new(t);
}
Type* type_fun_new(GList* params, Type* ret) {
      Type t;
      t.kind = TYPE_FUN;
      t.fun.params = params;
      t.fun.ret = ret;
      return type_new(t);
}

Type* type_ok_new(void) {
      Type t;
      t.kind = TYPE_OK;
      return type_new(t);
}

Type* type_conflict_new(void) {
      Type t;
      t.kind = TYPE_CONFLICT;
      return type_new(t);
}

/*** Accessors. ***/

int type_is_int(const Type* t) {
      return t && t->kind == TYPE_INT;
}

int type_is_bool(const Type* t) {
      return t && t->kind == TYPE_BOOL;
}

int type_is_nil(const Type* t) {
      return t && t->kind == TYPE_NIL;
}

int type_is_id(const Type* t) {
      return t && t->kind == TYPE_ID;
}

int type_is_struct(const Type* t) {
      return t && t->kind == TYPE_STRUCT;
}

int type_is_array(const Type* t) {
      return t && t->kind == TYPE_ARRAY;
}

int type_is_fun(const Type* t) {
      return t && t->kind == TYPE_FUN;
}

int type_is_void(const Type* t) {
      return t && t->kind == TYPE_VOID;
}

int type_is_ok(const Type* t) {
      return t && t->kind == TYPE_OK;
}

int type_is_conflict(const Type* t) {
      return t && t->kind == TYPE_CONFLICT;
}

int type_is_obj(const Type* t) {
      return t && (t->kind == TYPE_ARRAY
            || t->kind == TYPE_STRUCT
            || t->kind == TYPE_NIL
            || t->kind == TYPE_ID);
}

Type* type_fun_get_ret(Type* type) {
      assert(type);
      assert(type_is_fun(type));
      assert(type->fun.ret);
      return type->fun.ret;
}

GList* type_fun_get_params(Type* type) {
      assert(type);
      assert(type_is_fun(type));
      return type->fun.params;
}

const Type* type_array_get_eles(const Type* type) {
      assert(type);
      assert(type_is_array(type));
      assert(type->eles);
      return type->eles;
}

const Type* type_struct_get_field(const Type* type, Symbol id) {
      assert(type);
      assert(type_is_struct(type));
      assert(type->fields);
      assert(symbol_is_field(id));

      for (GList* itr = g_list_first(type->fields); itr; itr = g_list_next(itr)) {
            struct decl* field = (struct decl*)itr->data;
            if (symbol_equal(field->id, id)) return field->type;
      }

      return NULL;
}

static int types_equal(GList* left, GList* right) {
      if (left == right) return 1;
      if (!left || !right) return 0;
      if (g_list_length(left) != g_list_length(right))  return 0;

      left = g_list_first(left);
      right = g_list_first(right);
      while (left) {
            assert(left->data);
            assert(right->data);
            if (!type_equal((Type*)left->data, (Type*)right->data))
                  return 0;
            left = g_list_next(left);
            right = g_list_next(right);
      }
      return 1;
}

int type_equal(const Type* left, const Type* right) {
      if (left == right) return 1;
      if (!left || !right) return 0;
      if (left->kind != right->kind) return 0;

      switch (left->kind) { // == right->kind
            case TYPE_ID:
                  return symbol_equal(left->id, right->id);

            case TYPE_STRUCT:
                  return decls_equal(left->fields, right->fields);

            case TYPE_ARRAY:
                  return type_equal(left->eles, right->eles);

            case TYPE_FUN: {
                  GList* left_params = decls_to_types(left->fun.params);
                  GList* right_params = decls_to_types(right->fun.params);
                  int result = type_equal(left->fun.ret, right->fun.ret)
                        && types_equal(left_params, right_params);
                  g_list_free(left_params);
                  g_list_free(right_params);
                  return result;
            }

            default: return 1;
      }
}

Type* type_copy(const Type* t) {
      if (!t) return NULL;
      Type* copy = (Type*)calloc(1, sizeof(Type));
      assert(copy);

      *copy = *t;
      return copy;
}

Type* type_copy_deep(const Type* t) {
      if (!t) return NULL;
      Type* copy = (Type*)calloc(1, sizeof(Type));
      assert(copy);

      copy->kind = t->kind;

      switch (t->kind) {
            case TYPE_ID:
                  copy->id = t->id;
                  break;

            case TYPE_STRUCT:
                  copy->fields = list_copy_deep(t->fields, (CopyFunc)decl_copy_deep, NULL);
                  break;

            case TYPE_ARRAY:
                  copy->eles = type_copy_deep(t->eles);
                  break;

            case TYPE_FUN:
                  copy->fun.params = list_copy_deep(t->fun.params, (CopyFunc)decl_copy_deep, NULL);
                  copy->fun.ret = type_copy_deep(t->fun.ret);
                  break;
      }
      return copy;
}

GList* types_copy_deep(GList* types) {
      return list_copy_deep(types, (CopyFunc)type_copy, NULL);
}

void type_destroy(Type* t) {
      if (!t) return;

      switch (t->kind) {
            case TYPE_STRUCT:
                  list_free_full(t->fields, (DestroyFunc)decl_destroy);
                  break;

            case TYPE_ARRAY:
                  type_destroy(t->eles);
                  break;

            case TYPE_FUN:
                  list_free_full(t->fun.params, (DestroyFunc)decl_destroy);
                  type_destroy(t->fun.ret);
                  break;
      }
      free(t);
}

void types_destroy(GList* types) {
      list_free_full(types, (DestroyFunc)type_destroy);
}

void type_print_pretty(const Type* t) {
      if (!t) {
            printf("NONE");
            return;
      }
      switch (t->kind) {
            case TYPE_BOOL:
                  printf("bool");
                  break;
            case TYPE_INT:
                  printf("int");
                  break;
            case TYPE_NIL:
                  printf("nil");
                  break;
            case TYPE_ID:
                  printf("%s", symbol_to_str(t->id));
                  break;
            case TYPE_ARRAY:
                  printf("[");
                  type_print_pretty(t->eles);
                  printf("]");
                  break;
            case TYPE_STRUCT:
                  printf("{");
                  list_print_with_sep(t->fields, (PrintFunc)decl_print_pretty, ", ");
                  printf("}");
                  break;
            case TYPE_FUN:
                  printf("(");
                  list_print_with_sep(t->fun.params, (PrintFunc)decl_print_pretty, ", ");
                  printf(") -> ");
                  assert(t->fun.ret);
                  type_print_pretty(t->fun.ret);
                  break;
            case TYPE_VOID:
                  printf("void");
                  break;
            case TYPE_OK:
                  printf("ok");
                  break;
            case TYPE_CONFLICT:
                  printf("CONFLICT");
                  break;
      }
}

void type_print(const Type* t) {
      if (!t) {
            printf("(no-type)");
            return;
      }
      printf("(");
      switch (t->kind) {
            case TYPE_BOOL:
                  printf("type-bool");
                  break;
            case TYPE_INT:
                  printf("type-int");
                  break;
            case TYPE_NIL:
                  printf("type-nil");
                  break;
            case TYPE_ID:
                  printf("type-id %s", symbol_to_str(t->id));
                  break;
            case TYPE_ARRAY:
                  printf("type-array ");
                  type_print(t->eles);
                  break;
            case TYPE_STRUCT:
                  printf("type-struct ");
                  typed_ids_print(t->fields);
                  break;
            case TYPE_FUN:
                  printf("type-fun (params");
                  if (t->fun.params) printf(" ");
                  typed_ids_print(t->fun.params);
                  printf(") ");
                  type_print(t->fun.ret);
                  break;
            case TYPE_VOID:
                  printf("type-void");
                  break;
            case TYPE_OK:
                  printf("type-ok");
                  break;
            case TYPE_CONFLICT:
                  printf("type-conflict");
                  break;
      }
      printf(")");
}

void typed_ids_print(GList* decls) {
      list_print_with_sep(decls, (PrintFunc)typed_id_print, " ");
}

void typed_id_print(const struct decl* decl) {
      printf("(typed-id ");
      id_print(decl->id);
      printf(" ");
      type_print(decl->type);
      printf(")");
}

void decl_print_pretty(const struct decl* tid) {
      printf("%s : ", symbol_to_str(tid->id));
      type_print_pretty(tid->type);
}

static int decl_cmp(const struct decl* left, const struct decl* right) {
      assert(left);
      assert(right);
      return symbol_cmp(left->id, right->id);
}

static int decl_equal(const struct decl* left, const struct decl* right) {
      assert(left);
      assert(right);
      return symbol_equal(left->id, right->id) && type_equal(left->type, right->type);
}

static Type* decl_to_type(struct decl* decl) {
      assert(decl);
      return decl->type;
}

// Doesn't copy the types; use g_list_free().
GList* decls_to_types(GList* decls) {
      return list_copy_deep(decls, (CopyFunc)decl_to_type, NULL);
}

static int decls_equal(GList* left, GList* right) {
      if (left == right) return 1;
      if (!left || !right) return 0;
      if (g_list_length(left) != g_list_length(right))  return 0;

      left = g_list_first(left);
      right = g_list_first(right);
      while (left) {
            assert(left->data);
            assert(right->data);
            if (!decl_equal((struct decl*)left->data, (struct decl*)right->data))
                  return 0;
            left = g_list_next(left);
            right = g_list_next(right);
      }
      return 1;
}

// If the parameter is a typename, replace it with its declared type from the
// environment.
const Type* type_expand(const Type* type, Env* env) {
      assert(env);

      if (type_is_id(type)) {
            return (Type*)env_lookup(env, type->id);
      }
      return type;
}

int type_struct_field_offset(const Type* type, Symbol field) {
      assert(type);
      assert(type_is_struct(type));
      assert(symbol_is_field(field));

      int i = 0;
      for (GList* itr = g_list_first(type->fields); itr; itr = g_list_next(itr)) {
            if (((struct decl*)itr->data)->id.value == field.value) return i;
            ++i;
      }
      assert(!"type_struct_field_offset: field not in struct!");
      return 0;
}

