#include "env.h"
#include "type.h"
#include "ast.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

static int typed_id_cmp(const TypedId*, const TypedId*);
static Type* typed_id_to_type(TypedId* typed_id);

/*** Construction/allocation. ***/

Type type_int(void) {
      Type t;
      t.class = TYPE_INT;
      return t;
}

Type type_bool(void) {
      Type t;
      t.class = TYPE_BOOL;
      return t;
}

Type type_id(Symbol id) {
      Type t;
      t.class = TYPE_ID;
      t.id = id;
      return t;
}

Type type_struct(GList* fields) {
      Type t;
      t.class = TYPE_STRUCT;
      t.fields = g_list_sort(fields, (GCompareFunc)typed_id_cmp);
      return t;
}

Type type_array(Type* eles) {
      Type t;
      t.class = TYPE_ARRAY;
      t.eles = eles;
      return t;
}

Type type_fun(GList* params, Type* ret) {
      Type t;
      t.class = TYPE_FUN;
      t.fun.params = params;
      t.fun.ret = ret;
      return t;
}

Type type_void(void) {
      Type t;
      t.class = TYPE_VOID;
      return t;
}

Type type_nil(void) {
      Type t;
      t.class = TYPE_NIL;
      return t;
}

Type type_ok(void) {
      Type t;
      t.class = TYPE_OK;
      return t;
}

Type type_none(void) {
      Type t;
      t.class = TYPE_NONE;
      return t;
}

TypedId typed_id(Symbol id, Type* type) {
      TypedId tid;
      tid.id = id;
      tid.type = type;
      return tid;
}

Type* type_new(Type t) {
      Type* copy = malloc(sizeof(Type));
      *copy = t;
      return copy;
}

Type* type_copy(const Type* t) {
      if (!t) return NULL;
      Type* copy = malloc(sizeof(Type));
      *copy = *t;
      return copy;
}

TypedId* typed_id_new(TypedId t) {
      TypedId* copy = malloc(sizeof(TypedId));
      *copy = t;
      return copy;
}

TypedId* typed_id_copy(const TypedId* t) {
      if (!t) return NULL;
      TypedId* copy = malloc(sizeof(TypedId));
      *copy = *t;
      return copy;
}

/*** Accessors. ***/

int type_is_int(const Type* t) {
      return t && t->class == TYPE_INT;
}

int type_is_bool(const Type* t) {
      return t && t->class == TYPE_BOOL;
}

int type_is_id(const Type* t) {
      return t && t->class == TYPE_ID;
}

int type_is_struct(const Type* t) {
      return t && t->class == TYPE_STRUCT;
}

int type_is_array(const Type* t) {
      return t && t->class == TYPE_ARRAY;
}

int type_is_fun(const Type* t) {
      return t && t->class == TYPE_FUN;
}

int type_is_void(const Type* t) {
      return t && t->class == TYPE_VOID;
}

int type_is_nil(const Type* t) {
      return t && t->class == TYPE_NIL;
}

int type_is_ok(const Type* t) {
      return t && t->class == TYPE_OK;
}

int type_is_none(const Type* t) {
      return t && t->class == TYPE_NONE;
}

int type_is_obj(const Type* t) {
      return t && (t->class == TYPE_ARRAY
            || t->class == TYPE_STRUCT
            || t->class == TYPE_NIL
            || t->class == TYPE_ID);
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

Type* type_array_get_eles(Type* type) {
      assert(type);
      assert(type_is_array(type));
      assert(type->eles);
      return type->eles;
}

const Type* type_struct_get_field(Type* type, Symbol id) {
      assert(type);
      assert(type_is_struct(type));
      assert(type->fields);
      assert(symbol_is_field(id));

      GList* itr;
      for (itr = g_list_first(type->fields); itr; itr = g_list_next(itr)) {
            TypedId* field = itr->data;
            if (symbol_equal(field->id, id)) return field->type;
      }

      return NULL;
}

/*** Equality. ***/

int types_equal(GList* left, GList* right) {
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
      if (left->class != right->class) return 0;

      switch (left->class) { // == right->class
            case TYPE_ID:
                  return symbol_equal(left->id, right->id);

            case TYPE_STRUCT:
                  return typed_ids_equal(left->fields, right->fields);

            case TYPE_ARRAY:
                  assert(left->eles); assert(right->eles);
                  return type_equal(left->eles, right->eles);

            case TYPE_FUN: ;
                  GList* left_params = typed_ids_to_types(left->fun.params);
                  GList* right_params = typed_ids_to_types(right->fun.params);
                  int result = type_equal(left->fun.ret, right->fun.ret)
                        && types_equal(left_params, right_params);
                  types_destroy(left_params);
                  types_destroy(right_params);
                  return result;

            default: return 1;
      }
}

int typed_id_equal(const TypedId* left, const TypedId* right) {
      assert(left);
      assert(right);
      return symbol_equal(left->id, right->id) && type_equal(left->type, right->type);
}

int typed_ids_equal(GList* left, GList* right) {
      if (left == right) return 1;
      if (!left || !right) return 0;
      if (g_list_length(left) != g_list_length(right))  return 0;

      left = g_list_first(left);
      right = g_list_first(right);
      while (left) {
            assert(left->data);
            assert(right->data);
            if (!typed_id_equal((TypedId*)left->data, (TypedId*)right->data))
                  return 0;
            left = g_list_next(left);
            right = g_list_next(right);
      }
      return 1;
}

static int typed_id_cmp(const TypedId* left, const TypedId* right) {
      assert(left);
      assert(right);
      return symbol_cmp(left->id, right->id);
}

/*** Deep copy. ***/

Type* type_copy_deep(const Type* t) {
      if (!t) return NULL;
      Type* copy = calloc(1, sizeof(Type));
      assert(copy);
      copy->class = t->class;

      switch (t->class) {
            case TYPE_ID:
                  copy->id = t->id;
                  break;

            case TYPE_STRUCT:
                  copy->fields = list_copy_deep(t->fields, (CopyFunc)typed_id_copy_deep, NULL);
                  break;

            case TYPE_ARRAY:
                  copy->eles = type_copy_deep(t->eles);
                  break;

            case TYPE_FUN:
                  copy->fun.params = list_copy_deep(t->fun.params, (CopyFunc)typed_id_copy_deep, NULL);
                  copy->fun.ret = type_copy_deep(t->fun.ret);
                  break;
      }
      return copy;
}

GList* types_copy_deep(GList* types) {
      return list_copy_deep(types, (CopyFunc)type_copy, NULL);
}

TypedId* typed_id_copy_deep(const TypedId* t) {
      if (!t) return NULL;
      TypedId* copy = calloc(1, sizeof(TypedId));
      assert(copy);
      copy->id = t->id;
      copy->type = type_copy_deep(t->type);
      return copy;
}

static Type* typed_id_to_type(TypedId* typed_id) {
      assert(typed_id);
      return type_copy_deep(typed_id->type);
}

GList* typed_ids_to_types(GList* typed_ids) {
      return list_copy_deep(typed_ids, (CopyFunc)typed_id_to_type, NULL);
}

/*** Prfing. ***/

void type_print_pretty(const Type* t) {
      if (!t) {
            printf("(invalid) ");
            return;
      }
      switch (t->class) {
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
                  assert(t->eles);
                  type_print_pretty(t->eles);
                  printf("]");
                  break;
            case TYPE_STRUCT:
                  printf("{");
                  list_print_commad(t->fields, (PrintFunc)typed_id_print_pretty);
                  printf("}");
                  break;
            case TYPE_FUN:
                  printf("(");
                  list_print_commad(t->fun.params, (PrintFunc)typed_id_print_pretty);
                  printf(") -> ");
                  assert(t->fun.ret);
                  type_print_pretty(t->fun.ret);
                  break;
            case TYPE_VOID:
                  printf("void");
                  break;
      }
}

void type_print(const Type* t) {
      if (!t) {
            printf("(no-type) ");
            return;
      }
      printf("(");
      switch (t->class) {
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
                  assert(t->eles);
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
                  assert(t->fun.ret);
                  printf(") ");
                  type_print(t->fun.ret);
                  break;
            case TYPE_VOID:
                  printf("type-void");
                  break;
      }
      printf(")");
}

void types_print(GList* types) {
      printf("(types");
      if (types) printf(" ");
      list_print_spaced(types, (PrintFunc)type_print);
      printf(")");
}

void typed_ids_print(GList* typed_ids) {
      list_print_spaced(typed_ids, (PrintFunc)typed_id_print);
}

void typed_id_print(const TypedId* tid) {
      printf("(typed-id ");
      id_print(tid->id);
      printf(" ");
      type_print(tid->type);
      printf(")");
}

void typed_id_print_pretty(const TypedId* tid) {
      printf("%s : ", symbol_to_str(tid->id));
      type_print_pretty(tid->type);
}

/*** Destruction. ***/

void type_destroy(Type* t) {
      if (!t) return;

      switch (t->class) {
            case TYPE_STRUCT:
                  list_free_full(t->fields, (DestroyFunc)typed_id_destroy);
                  break;

            case TYPE_ARRAY:
                  type_destroy(t->eles);
                  break;

            case TYPE_FUN:
                  list_free_full(t->fun.params, (DestroyFunc)typed_id_destroy);
                  type_destroy(t->fun.ret);
                  break;
      }
      free(t);
}

void types_destroy(GList* types) {
      list_free_full(types, (DestroyFunc)type_destroy);
}

void typed_id_destroy(TypedId* t) {
      if (t) {
            type_destroy(t->type);
            free(t);
      }
}

