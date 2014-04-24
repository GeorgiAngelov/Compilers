#include "ast.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

static struct decl* decl_new(Symbol id, Type* type, struct exp* exp, GList* decls, GList* stmts);
static struct stmt* stmt_new(int kind);
static void binop_print(const char* op, const Type* type, struct exp* left, struct exp* right);

#define SHIFTWIDTH 3
static int indent;

static void indent_enter(void) {
      if (indent) puts("");
      for (int i = 0; i != indent; ++i) printf(" ");
      indent += SHIFTWIDTH;
}

static void indent_exit(void) {
      indent -= SHIFTWIDTH;
}

/*** Declarations. ***/

static struct decl* decl_new(Symbol id, Type* type, struct exp* exp, GList* decls, GList* stmts) {
      struct decl* me = (struct decl*)calloc(1, sizeof(struct decl));
      assert(me);

      me->id = id;
      me->type = type;
      me->exp = exp;
      me->decls = decls;
      me->stmts = stmts;
      return me;
}

struct decl* decl_field_new(const char* id, Type* type) {
      return decl_new(symbol_field(id), type, NULL, NULL, NULL);
}

struct decl* decl_param_new(const char* id, Type* type) {
      return decl_new(symbol_var(id), type, NULL, NULL, NULL);
}

struct decl* decl_var_new(const char* id, Type* type, struct exp* exp) {
      return decl_new(symbol_var(id), type, exp, NULL, NULL);
}

struct decl* decl_typename_new(const char* id, Type* type) {
      return decl_new(symbol_typename(id), type, NULL, NULL, NULL);
}

struct decl* decl_fun_new(const char* id, Type* type, GList* decls, GList* stmts) {
      return decl_new(symbol_fun(id), type, NULL, decls, stmts);
}

void decl_print(struct decl* d) {
      assert(d);

      indent_enter();

      printf("(");

      if (symbol_is_var(d->id)) {
            printf("decl-var:");
      } else if (symbol_is_fun(d->id)) {
            printf("decl-fun:");
      } else if (symbol_is_typename(d->id)) {
            printf("decl-type:");
      }
      type_print_pretty(d->node_type);
      printf(" ");

      id_print(d->id);
      printf(" ");
      type_print(d->type);

      if (d->exp) {
            printf(" ");
            exp_print(d->exp);
      } else if (d->decls || d->stmts) {
            decls_print(d->decls);
            stmts_print(d->stmts);
      }
      printf(")");
      indent_exit();
}

void decls_print(GList* decls) {
      indent_enter();
      printf("(decls");
      g_list_foreach(decls, (GFunc)decl_print, NULL);
      printf(")");
      indent_exit();
}

struct decl* decl_copy_deep(const struct decl* d) {
      if (!d) return NULL;
      struct decl* me = decl_new(d->id,
            type_copy_deep(d->type), exp_copy_deep(d->exp),
            decls_copy_deep(d->decls), stmts_copy_deep(d->stmts));
      me->node_type = type_copy_deep(d->node_type);
      return me;
}

GList* decls_copy_deep(GList* decls) {
      return list_copy_deep(decls, (CopyFunc)decl_copy_deep, NULL);
}

void decl_destroy(struct decl* d) {
      if (!d) return;
      type_destroy(d->type);
      exp_destroy(d->exp);
      decls_destroy(d->decls);
      stmts_destroy(d->stmts);
      type_destroy(d->node_type);
      free(d);
}

void decls_destroy(GList* decls) {
      list_free_full(decls, (DestroyFunc)decl_destroy);
}

/*** Expressions. ***/

struct exp* exp_new(int kind) {
      struct exp* me = (struct exp*)calloc(1, sizeof(struct exp));
      assert(me);

      me->kind = kind;
      return me;
}

struct exp* exp_binop_new(int kind, struct exp* left, struct exp* right) {
      struct exp* me = exp_new(kind);
      me->left = left;
      me->right = right;
      return me;
}

struct exp* exp_not_new(struct exp* right) {
      return exp_binop_new(AST_EXP_NOT, NULL, right);
}

struct exp* exp_num_new(int num) {
      struct exp* me = exp_new(AST_EXP_NUM);
      me->num = num;
      return me;
}

struct exp* exp_str_new(char* str) {
      struct exp* me = exp_new(AST_EXP_STR);
      me->str = str;
      return me;
}

struct exp* exp_id_new(Symbol id) {
      struct exp* me = exp_new(AST_EXP_ID);
      me->id = id;
      return me;
}

struct exp* exp_struct_lit_new(GList* field_inits) {
      struct exp* me = exp_new(AST_EXP_STRUCT_LIT);
      me->params = field_inits;
      return me;
}

struct exp* exp_array_lit_new(GList* exps) {
      struct exp* me = exp_new(AST_EXP_ARRAY_LIT);
      me->params = exps;
      return me;
}

struct exp* exp_fun_call_new(Symbol id, GList* params) {
      struct exp* me = exp_new(AST_EXP_FUN_CALL);
      me->id = id;
      me->params = params;
      return me;
}

struct exp* exp_array_index_new(struct exp* arr, struct exp* idx) {
      return exp_binop_new(AST_EXP_ARRAY_INDEX, arr, idx);
}

struct exp* exp_field_lookup_new(struct exp* strct, Symbol field) {
      struct exp* me = exp_new(AST_EXP_FIELD_LOOKUP);
      me->left = strct;
      me->id = field;
      return me;
}

static void binop_print(const char* op, const Type* type, struct exp* left, struct exp* right) {
      printf("%s:", op);
      type_print_pretty(type);
      printf(" ");
      exp_print(left);
      printf(" ");
      exp_print(right);
}

void id_print(Symbol symbol) {
      printf("(id %s)", symbol_to_str(symbol));
}

void exp_print(struct exp* exp) {
      if (!exp) return;
      printf("(");
      switch (exp->kind) {
            case AST_EXP_PLUS:
                  binop_print("+", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_MINUS:
                  binop_print("-", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_DIV:
                  binop_print("/", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_MOD:
                  binop_print("%", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_MUL:
                  binop_print("*", exp->node_type, exp->left, exp->right);
                  break;

            case AST_EXP_OR:
                  binop_print("|", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_AND:
                  binop_print("&", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_NOT:
                  printf("!:");
                  type_print_pretty(exp->node_type);
                  exp_print(exp->right);
                  break;
            case AST_EXP_LT:
                  binop_print("<", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_LT_EQ:
                  binop_print("<=", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_GT:
                  binop_print(">", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_GT_EQ:
                  binop_print(">=", exp->node_type, exp->left, exp->right);
                  break;

            case AST_EXP_EQ:
                  binop_print("==", exp->node_type, exp->left, exp->right);
                  break;
            case AST_EXP_NOT_EQ:
                  binop_print("!=", exp->node_type, exp->left, exp->right);
                  break;

            case AST_EXP_NUM:
                  printf("num:int %d", exp->num);
                  break;
            case AST_EXP_TRUE:
                  printf("true:bool");
                  break;
            case AST_EXP_FALSE:
                  printf("false:bool");
                  break;
            case AST_EXP_NIL:
                  printf("nil:nil");
                  break;
            case AST_EXP_STR:
                  printf("str:[int] \"%s\"", exp->str);
                  break;
            case AST_EXP_ID:
                  printf("id:");
                  type_print_pretty(exp->node_type);
                  printf(" %s", symbol_to_str(exp->id));
                  break;

            case AST_EXP_STRUCT_LIT: {
                  printf("struct-lit:");
                  type_print_pretty(exp->node_type);
                  printf(" ");
                  field_inits_print(exp->params);
                  break;
            }

            case AST_EXP_ARRAY_LIT: {
                  printf("array-lit:");
                  type_print_pretty(exp->node_type);
                  printf(" ");
                  exps_print(exp->params);
                  break;
            }

            case AST_EXP_FUN_CALL: {
                  printf("fun-call:");
                  type_print_pretty(exp->node_type);
                  printf(" ");
                  id_print(exp->id);
                  printf(" ");
                  exps_print(exp->params);
                  break;
            }

            case AST_EXP_ARRAY_INDEX: {
                  printf("array-idx:");
                  type_print_pretty(exp->node_type);
                  printf(" ");
                  exp_print(exp->left);
                  printf(" ");
                  exp_print(exp->right);
                  break;
            }

            case AST_EXP_FIELD_LOOKUP: {
                  printf("field-lkup:");
                  type_print_pretty(exp->node_type);
                  printf(" ");
                  exp_print(exp->left);
                  printf(" ");
                  id_print(exp->id);
            }
      }
      printf(")");
}

void exps_print(GList* exps) {
      printf("(exps");
      if (exps) printf(" ");
      list_print_with_sep(exps, (PrintFunc)exp_print, " ");
      printf(")");
}

struct exp* exp_copy_deep(const struct exp* exp) {
      if (!exp) return NULL;

      struct exp* me = exp_new(exp->kind);

      me->num = exp->num;      
      me->id = exp->id;
      if (exp->str) me->str = strdup(exp->str);
      me->left = exp_copy_deep(exp->left);
      me->right = exp_copy_deep(exp->right);
      me->node_type = type_copy_deep(exp->node_type);

      switch (exp->kind) {
            case AST_EXP_STRUCT_LIT:
                  me->params = field_inits_copy_deep(exp->params);
                  break;
            case AST_EXP_ARRAY_LIT:
            case AST_EXP_FUN_CALL:
                  me->params = exps_copy_deep(exp->params);
                  break;
      }
      return me;
}

GList* exps_copy_deep(GList* exps) {
      return list_copy_deep(exps, (CopyFunc)exp_copy_deep, NULL);
}

void exp_destroy(struct exp* exp) {
      if (!exp) return;

      if (exp->str) free(exp->str);
      exp_destroy(exp->left);
      exp_destroy(exp->right);
      type_destroy(exp->node_type);

      switch (exp->kind) {
            case AST_EXP_STRUCT_LIT:
                  field_inits_destroy(exp->params);
                  break;
            case AST_EXP_ARRAY_LIT:
            case AST_EXP_FUN_CALL:
                  exps_destroy(exp->params);
                  break;
      }
      free(exp);
}

void exps_destroy(GList* exps) {
      list_free_full(exps, (DestroyFunc)exp_destroy);
}

/*** Struct field initializers. ***/

struct field_init* field_init_new(Symbol field, struct exp* init) {
      struct field_init* me = (struct field_init*)calloc(1, sizeof(struct field_init));
      assert(me);

      me->id = field;
      me->init = init;
      return me;
}

void field_init_print(struct field_init* fi) {
      assert(fi);

      printf("(field-init ");
      id_print(fi->id);
      printf(" ");
      exp_print(fi->init);
      printf(")");
}

void field_inits_print(GList* field_inits) {
      printf("(field-inits ");
      list_print_with_sep(field_inits, (PrintFunc)field_init_print, " ");
      printf(")");
}

struct field_init* field_init_copy_deep(const struct field_init* fi) {
      if (!fi) return NULL;
      struct field_init* me = field_init_new(fi->id, fi->init);
      me->init = exp_copy_deep(fi->init);
      return me;
}

GList* field_inits_copy_deep(GList* fis) {
      return list_copy_deep(fis, (CopyFunc)field_init_copy_deep, NULL);
}

void field_init_destroy(struct field_init* fi) {
      if (!fi) return;
      exp_destroy(fi->init);
      free(fi);
}

void field_inits_destroy(GList* fis) {
      list_free_full(fis, (DestroyFunc)field_init_destroy);
}

/*** Statements. ***/

static struct stmt* stmt_new(int kind) {
      struct stmt* me = (struct stmt*)calloc(1, sizeof(struct stmt));
      assert(me);

      me->kind = kind;
      return me;
}

struct stmt* stmt_exp_new(struct exp* exp) {
      struct stmt* me = stmt_new(AST_STMT_EXP);
      me->exp = exp;
      return me;
}

struct stmt* stmt_assign_new(struct exp* left, struct exp* right) {
      struct stmt* me = stmt_new(AST_STMT_ASSIGN);
      me->left = left;
      me->right = right;
      return me;
}

struct stmt* stmt_if_new(struct exp* cond, GList* then_block, GList* else_block) {
      struct stmt* me = stmt_new(AST_STMT_IF);
      me->exp = cond;
      me->block1 = then_block;
      me->block2 = else_block;
      return me;
}

struct stmt* stmt_while_new(struct exp* cond, GList* block) {
      struct stmt* me = stmt_new(AST_STMT_WHILE);
      me->exp = cond;
      me->block1 = block;
      return me;
}

struct stmt* stmt_for_new(struct exp* lval, struct exp* from, struct exp* to, GList* block) {
      struct stmt* me = stmt_new(AST_STMT_FOR);
      me->left = lval;
      me->exp = from;
      me->right = to;
      me->block1 = block;
      return me;
}

struct stmt* stmt_return_new(struct exp* exp) {
      struct stmt* me = stmt_new(AST_STMT_RETURN);
      me->exp = exp;
      return me;
}

void stmt_print(struct stmt* stmt) {
      assert(stmt);

      indent_enter();
      printf("(");
      switch (stmt->kind) {
            case AST_STMT_EXP: {
                  printf("exp-stmt:");
                  type_print_pretty(stmt->node_type);
                  if (stmt->exp) printf(" ");
                  exp_print(stmt->exp);
                  break;
            }

            case AST_STMT_ASSIGN: {
                  printf("assign:");
                  type_print_pretty(stmt->node_type);
                  printf(" ");
                  exp_print(stmt->left);
                  printf(" ");
                  exp_print(stmt->right);
                  break;
            }

            case AST_STMT_IF: {
                  printf("if:");
                  type_print_pretty(stmt->node_type);
                  printf(" ");
                  exp_print(stmt->exp);
                  stmts_print(stmt->block1);
                  stmts_print(stmt->block2);
                  break;
            }

            case AST_STMT_WHILE: {
                  printf("while:");
                  type_print_pretty(stmt->node_type);
                  printf(" ");
                  exp_print(stmt->exp);
                  stmts_print(stmt->block1);
                  break;
            }

            case AST_STMT_FOR: {
                  printf("for:");
                  type_print_pretty(stmt->node_type);
                  printf(" ");
                  exp_print(stmt->left);
                  printf(" ");
                  exp_print(stmt->exp);
                  printf(" ");
                  exp_print(stmt->right);
                  stmts_print(stmt->block1);
                  break;
            }

            case AST_STMT_RETURN: {
                  printf("return:");
                  type_print_pretty(stmt->node_type);
                  if (stmt->exp) {
                        printf(" ");
                        exp_print(stmt->exp);
                  }
                  break;
            }
      }
      printf(")");
      indent_exit();
}

void stmts_print(GList* stmts) {
      indent_enter();
      printf("(stmts");
      g_list_foreach(stmts, (GFunc)stmt_print, NULL);
      printf(")");
      indent_exit();
}

struct stmt* stmt_copy_deep(const struct stmt* stmt) {
      if (!stmt) return NULL;
      struct stmt* me = stmt_new(stmt->kind);
      me->left = exp_copy_deep(stmt->left);
      me->exp = exp_copy_deep(stmt->exp);
      me->right = exp_copy_deep(stmt->right);
      me->block1 = stmts_copy_deep(stmt->block1);
      me->block2 = stmts_copy_deep(stmt->block2);
      me->node_type = type_copy_deep(stmt->node_type);
      return me;
}

GList* stmts_copy_deep(GList* stmts) {
      return list_copy_deep(stmts, (CopyFunc)stmt_copy_deep, NULL);
}

void stmt_destroy(struct stmt* stmt) {
      if (!stmt) return;
      exp_destroy(stmt->left);
      exp_destroy(stmt->exp);
      exp_destroy(stmt->right);
      stmts_destroy(stmt->block1);
      stmts_destroy(stmt->block2);
      type_destroy(stmt->node_type);
      free(stmt);
}

void stmts_destroy(GList* stmts) {
      list_free_full(stmts, (DestroyFunc)stmt_destroy);
}

/*** Bonus list manipulation functions. ***/

// Defined in later versions of glib.
GList* list_copy_deep(GList* list, CopyFunc copy, void* user_data) {
      assert(copy);
      GList* new_list = NULL;
      GList* itr;
      for (itr = g_list_first(list); itr; itr = g_list_next(itr)) {
            new_list = g_list_append(new_list, copy(itr->data, user_data));
      }
      return new_list;
}

// Defined in later versions of glib.
void list_free_full(GList* list, DestroyFunc destroy) {
      assert(destroy);
      list = g_list_first(list);
      g_list_foreach(list, (GFunc)destroy, NULL);
      g_list_free(list);
}

void list_print_with_sep(GList* list, PrintFunc print, const char* sep) {
      GList* itr;
      for (itr = g_list_first(list); itr != g_list_last(itr); itr = g_list_next(itr)) {
            print(itr->data);
            printf(sep);
      }
      if (itr) print(itr->data);
}

