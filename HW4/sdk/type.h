#ifndef LIGER_TYPE_H_
#define LIGER_TYPE_H_

typedef struct type_ Type;

#include "env.h"
#include <glib.h>

enum {
      TYPE_INT,
      TYPE_BOOL,
      TYPE_ID,
      TYPE_STRUCT,
      TYPE_ARRAY,
      TYPE_FUN,
      TYPE_VOID,
      TYPE_NIL,
      TYPE_OK,
      TYPE_CONFLICT, // For conflicting decls.
};

struct type_ {
      int kind;

      union {
            // Extra info for composite types.
            Symbol id; // Typename.
            Type* eles; // Arrays.

            // Sorted TypedId list (for structs) or Type list (for array lits).
            GList* fields;

            struct {
                  GList* params; // Function TypedId list.
                  Type* ret; // Function return type.
            } fun;
      };
};

void type_print(const Type*);
void type_print_pretty(const Type*);

int type_is_array(const Type*);
int type_is_array_lit(const Type*);
int type_is_bool(const Type*);
int type_is_conflict(const Type*);
int type_is_fun(const Type*);
int type_is_id(const Type*);
int type_is_int(const Type*);
int type_is_nil(const Type*);
int type_is_obj(const Type*);
int type_is_ok(const Type*);
int type_is_struct(const Type*);
int type_is_void(const Type*);

// Constructors.
Type type(int kind);
Type* type_int_new(void);
Type* type_bool_new(void);
Type* type_conflict_new(void);
Type* type_nil_new(void);
Type* type_void_new(void);
Type* type_id_new(Symbol id);
Type* type_struct_new(GList* fields);
Type* type_array_new(Type* eles);
Type* type_fun_new(GList* params, Type* ret);
Type* type_ok_new(void);

Type* type_fun_get_ret(Type*);
GList* type_fun_get_params(Type*);
const Type* type_array_get_eles(const Type*);
const Type* type_struct_get_field(const Type*, Symbol);

// Create a shallow copy in malloc'd memory. Use free() when done.
Type* type_copy(const Type*);

// Create a deep copy in malloc'd memory. Use *_destroy() when done.
Type* type_copy_deep(const Type*);
GList* types_copy_deep(GList*);

int type_equal(const Type*, const Type*);

// Returns a new, deep-copied list.
GList* decls_to_types(GList*);

// Recursively frees all memory.
void type_destroy(Type*);
void types_destroy(GList*);

int type_struct_field_offset(const Type*, Symbol field);

// If the parameter is a typename, replace it with its declared type from the
// environment.
const Type* type_expand(const Type* type, Env* env);

#endif
