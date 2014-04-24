#ifndef LIGER_ENV_H_
#define LIGER_ENV_H_

typedef struct symbol_ {
      int kind;
      int value;
} Symbol;
typedef struct env_ Env;

#include "type.h"
#include <glib.h>

enum {
      BASE_INVALID,
      BASE_GVAR = 256,
      BASE_PARAM,
      BASE_LVAR,
};

struct env_ {
      GHashTable* funs;
      GHashTable* typenames;
      GHashTable* vars;
};

typedef struct loc_ {
      int base;
      int offset;
} Loc;

/*** Symbols ***/

// These functions translate a string into a symbol unique to that string. The
// resulting symbol can be quickly compared for equality and the like and it
// also carries around its namespace (i.e., whether it's a struct field,
// function name, type name, or regular variable name).
Symbol symbol_field(const char* id);
Symbol symbol_fun(const char* id);
Symbol symbol_fun_return(void);
Symbol symbol_typename(const char* id);
Symbol symbol_var(const char* id);

// Translates a symbol to the original string used to create it. The resulting
// string shouldn't be messed with.
const char* symbol_to_str(Symbol);

// Check the namespace of a symbol.
int symbol_is_valid(Symbol);
int symbol_is_storable(Symbol);
int symbol_is_field(Symbol);
int symbol_is_fun(Symbol);
int symbol_is_var(Symbol);
int symbol_is_pseudo(Symbol);
int symbol_is_typename(Symbol);

// Symbol equality and comparison (a la strcmp).
int symbol_equal(Symbol, Symbol);
int symbol_cmp(Symbol, Symbol);

void symbol_print(Symbol);

/*** Environments ***/

Env* env_new(void);
Env* env_copy_deep(Env* env);

int env_contains(Env*, Symbol);

Type* env_lookup(Env*, Symbol);

// Associates a symbol with the parameter type. Creates a deep copy of the type
// to store in the environment, which can be freed with env_destroy().
void env_insert(Env*, Symbol, Type*);
// Store extra data for functions. Again, inserts a deep copy of everything.
void env_insert_fun(Env*, Symbol, Type*, Env*, GList*);

// Returns data associated with the parameter symbol. These are the actual
// pointers stored in the environment and not copies, so don't free them except
// through env_destroy().
Env* env_lookup_fun_env(Env*, Symbol);
GList* env_lookup_fun_def(Env*, Symbol);

// Returns a new environment, the sorta-union of the parameter environments.
// Key/value pairs in the second environment will over-write those in the
// first.
Env* env_union(Env* left, Env* right);

void env_print(Env*);

// Recursively frees all memory associated with this hash table.
void env_destroy(Env* env);

#endif
