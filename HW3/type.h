#ifndef LIGER_TYPE_H_
#define LIGER_TYPE_H_

typedef struct type_ Type;
typedef struct typed_id_ TypedId;

#include "env.h"
#include <glib.h>

enum {
      TYPE_INT,
      TYPE_BOOL,
      TYPE_ID,
      TYPE_STRUCT,
      TYPE_ARRAY,
      TYPE_FUN,
      TYPE_VOID, // Type of a function that doesn't return anything.
      TYPE_NIL,  // Type of the "nil" expression.
      TYPE_OK,   // Type of statements and declarations.
};

struct type_ {
      int class;

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

// TypedIds are (id, type) tuples that appear in various places (e.g., function
// parameters, struct field types).
struct typed_id_ {
      Symbol id;
      Type* type;
};

// These functions do NOT copy anything or allocate any memory, use type_new()
// for a new allocation.
Type type_int(void);
Type type_bool(void);
Type type_id(Symbol id);
Type type_struct(GList* fields);
Type type_array(Type* eles);
Type type_fun(GList* params, Type* ret);
Type type_void(void);
Type type_nil(void);
Type type_ok(void);
TypedId typed_id(Symbol id, Type* type);

// Create a shallow copy in malloc'd memory. Use free() when done. *_new()
// is the same as *_copy(), but with different arguments.
Type* type_new(Type);
Type* type_copy(const Type*);
TypedId* typed_id_new(TypedId);
TypedId* typed_id_copy(const TypedId*);

// Check the class of a type.
int type_is_int(const Type*);
int type_is_bool(const Type*);
int type_is_nil(const Type*);
int type_is_id(const Type*);
int type_is_struct(const Type*);
int type_is_array(const Type*);
int type_is_array_lit(const Type*);
int type_is_fun(const Type*);
int type_is_void(const Type*);
int type_is_ok(const Type*);
int type_is_obj(const Type*);

// Accessors for composite types.
Type* type_fun_get_ret(Type*);
GList* type_fun_get_params(Type*);
Type* type_array_get_eles(Type*);
// Lookup the type of a field in a struct type.
const Type* type_struct_get_field(Type*, Symbol);

// Create a deep copy in malloc'd memory. Use *_destroy() when done.
Type* type_copy_deep(const Type*);
GList* types_copy_deep(GList*);
TypedId* typed_id_copy_deep(const TypedId*);

// Returns a list of Types copied from a list of TypedIds. Useful, for example,
// to extract a list of function param types from the TypedId list that they're
// stored as. Use types_destroy() when done.
GList* typed_ids_to_types(GList* typed_ids);

// Recursive equality.
int type_equal(const Type*, const Type*);
int types_equal(GList*, GList*);
int typed_id_equal(const TypedId*, const TypedId*);
int typed_ids_equal(GList*, GList*);

// Print. The *_pretty versions print in Liger concrete syntax.
void type_print(const Type*);
void type_print_pretty(const Type*);
void types_print(GList*);
void typed_id_print(const TypedId*);
void typed_id_print_pretty(const TypedId*);
void typed_ids_print(GList* typed_ids);

// Recursively frees all memory.
void type_destroy(Type*);
void types_destroy(GList*);
void typed_id_destroy(TypedId*);

#endif
