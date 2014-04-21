#ifndef LIGER_AST_H_
#define LIGER_AST_H_

#include "type.h"
#include "env.h"
#include <glib.h>

// These are a set of Liger AST nodes. The 3 main kinds: declarations,
// statements, and expressions. But we use other structures too that don't quite
// seem worthy of being called nodes: Type, TypedId, and field_init, along with
// lists (GList) of all these things.
//
// In the structs for each node, the different fields mean different things
// depending on the "class" of the node.

/*** Declarations. ***/

struct decl {
      // The "class" of decl (var, typename, function) is carried around in the
      // symbol namespace, so we don't need a separate "class" field.
      Symbol id;
      int status;
      
      Type* type; // The type also carries around function parameters.
      // Possible initializers.
      struct exp* exp; // For a "var" decl.
      GList* decls;    // The decls in the body of a function def.
      GList* stmts;    // The stmts in the body of a function def.
};

/*** Expressions. ***/

enum ast_exp_class {
      AST_EXP_PLUS = 256,
      AST_EXP_MINUS,
      AST_EXP_DIV,
      AST_EXP_MOD,
      AST_EXP_MUL,

      AST_EXP_OR,
      AST_EXP_AND,
      AST_EXP_NOT,

      AST_EXP_LT,
      AST_EXP_LT_EQ,
      AST_EXP_GT,
      AST_EXP_GT_EQ,

      AST_EXP_EQ,
      AST_EXP_NOT_EQ,

      AST_EXP_NUM,
      AST_EXP_TRUE,
      AST_EXP_FALSE,
      AST_EXP_NIL,
      AST_EXP_STR,
      AST_EXP_ID,

      AST_EXP_STRUCT_LIT,
      AST_EXP_ARRAY_LIT,

      AST_EXP_FUN_CALL,
      AST_EXP_ARRAY_INDEX,
      AST_EXP_FIELD_LOOKUP,
};

struct exp {
      int class;
      int status;

      int num;
      Symbol id;
      char* str;

      struct exp* left;
      struct exp* right;

      // Exp (function call, array lit) or field_init (struct lit) list.
      GList* params;
};

// A struct literal is a list (GList) of id, initializer pairs. The initializer
// can be an arbitrary expression.
struct field_init {
      Symbol id;
      struct exp* init;
};

enum ast_stmt_class {
      AST_STMT_EXP = 256,
      AST_STMT_ASSIGN,

      AST_STMT_IF,

      AST_STMT_WHILE,
      AST_STMT_FOR,

      AST_STMT_RETURN,
};

struct stmt {
      int class;
      struct exp* left;
      // Intended for the condtional exp in if, while, and "from" exp in for.
      struct exp* exp;
      struct exp* right;
      GList* block1; // Stmt list
      GList* block2;
};

// All of these constructors allocate memory for the various AST nodes, but
// don't deep copy their parameters.

/*** Declaration constructor ***/

// Constructor for decls. The third parameter is for an initializer (NULL if
// none) and the last two are only for function defs.
struct decl* decl_new(Symbol, Type*, struct exp*, GList* decls, GList* stmts);

/*** Expression constructors. ***/

// This is the basic exp constructor, useful for nodes which don't need to
// store any more extra data in the node (boolean literals, for example).
struct exp* exp_new(int class);
// Constructor for all binary operations.
struct exp* exp_binop_new(int class, struct exp* left, struct exp* right);
// Boolean not. Use the binop constructor for unary + and -.
struct exp* exp_not_new(struct exp* right);
// Numeric literal.
struct exp* exp_num_new(int num);
// String literal.
struct exp* exp_str_new(char* str);
// Identifier.
struct exp* exp_id_new(Symbol id);
// Struct literal. The parameter is a list of the field_init structures that
// appear above.
struct exp* exp_struct_lit_new(GList* field_inits);
// Array literal. The parameter is a list of exp structs.
struct exp* exp_array_lit_new(GList* exps);
// Function call. The first parameter is the function identifier and the second
// is a list of exp structs, the function parameters.
struct exp* exp_fun_call_new(Symbol id, GList* params);
// Array subscripting. The first parameter is the array expression, the second
// is the index expression.
struct exp* exp_array_index_new(struct exp* arr, struct exp* idx);
// Stuct field lookup. The first parameter is the struct expression, the second
// is the field being looked up.
struct exp* exp_field_lookup_new(struct exp* strct, Symbol field);

/*** Statement constructors. ***/

// Expression statement.
struct stmt* stmt_exp_new(struct exp* exp);
// Assignment.
struct stmt* stmt_assign_new(struct exp* left, struct exp* right);
// If-then, if-then-else (use NULL for missing branch). The blocks are lists of
// struct stmts.
struct stmt* stmt_if_new(struct exp* cond, GList* then_block, GList* else_block);
// While loop. The block is a list of stmts.
struct stmt* stmt_while_new(struct exp* cond, GList* block);
// For loop. The block is a list of stmts.
struct stmt* stmt_for_new(struct exp* lval, struct exp* from, struct exp* to, GList* block);
// Return. Use NULL for the no-argument version.
struct stmt* stmt_return_new(struct exp* exp);

// Used in struct literals: {field1 = init1, field2 = init2, ... }
struct field_init* field_init_new(Symbol field, struct exp* init);

// Print.
void decl_print(struct decl*);
void decls_print(GList*);
void exp_print(struct exp*);
void exps_print(GList*);
void id_print(Symbol);
void field_init_print(struct field_init*);
void field_inits_print(GList*);
void stmt_print(struct stmt*);
void stmts_print(GList*);

// Recursively copy all memory associated with an AST node.
struct decl* decl_copy_deep(struct decl*);
GList* decls_copy_deep(GList*);
struct exp* exp_copy_deep(struct exp*);
GList* exps_copy_deep(GList*);
struct field_init* field_init_copy_deep(struct field_init*);
GList* field_inits_copy_deep(GList*);
struct stmt* stmt_copy_deep(struct stmt*);
GList* stmts_copy_deep(GList*);

// Recursively free all memory associated with an AST node.
void decl_destroy(struct decl*);
void decls_destroy(GList*);
void exp_destroy(struct exp*);
void exps_destroy(GList*);
void field_init_destroy(struct field_init*);
void field_inits_destroy(GList*);
void stmt_destroy(struct stmt*);
void stmts_destroy(GList*);

// Generic list manipulation functions.
typedef void* (*CopyFunc)(void*, void*);
GList* list_copy_deep(GList*, CopyFunc, void*);
typedef void (*DestroyFunc)(void*);
void list_free_full(GList*, DestroyFunc);
typedef void (*PrintFunc)(void*);
void list_print_spaced(GList*, PrintFunc);
void list_print_commad(GList*, PrintFunc);
void list_print_with_sep(GList*, PrintFunc, const char* sep);



///THIS IS FOR TYPECHECKING!!!!!!!!!!!!!!!
// Print.
void decl_print_type(struct decl*, Env*);
void decls_print_type(GList*, Env*);
void exp_print_type(struct exp*, Env*);
void exps_print_type(GList*, Env*);
void id_print_type(Symbol, Env*);
void field_init_print_type(struct field_init*, Env*);
void field_inits_print_type(GList*, Env*);
void stmt_print_type(struct stmt*, Env*);
void stmts_print_type(GList*, Env*);

void list_print_spaced_type(GList*, PrintFunc, Env*);
void list_print_commad_type(GList*, PrintFunc, Env*);
void list_print_with_sep_type(GList*, PrintFunc, const char* sep, Env*);

#endif
