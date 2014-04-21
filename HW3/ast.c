#include "ast.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

static struct stmt* stmt_new(int class);
static void binop_print(const char* op, struct exp* left, struct exp* right);
static void binop_print_type(const char* op, struct exp* left, struct exp* right, Env*);

#define SHIFTWIDTH 3
static int indent;

static void indent_enter(void) {
      int i = 0;
      if (indent) puts("");
      for (; i != indent; ++i) printf(" ");
      indent += SHIFTWIDTH;
}

static void indent_exit(void) {
      indent -= SHIFTWIDTH;
}

/*** Declarations. ***/

struct decl* decl_new(Symbol id, Type* type, struct exp* exp, GList* decls, GList* stmts) {
      struct decl* d = calloc(1, sizeof(struct decl));
      d->id = id;
      d->type = type;
      d->exp = exp;
      d->decls = decls;
      d->stmts = stmts;
      return d;
}

void decl_print(struct decl* d) {
      assert(d);
      assert(symbol_is_storable(d->id));

      indent_enter();

      printf("(");

      if (symbol_is_var(d->id)) {
            printf("decl-var ");
      } else if (symbol_is_fun(d->id)) {
            printf("decl-fun ");
      } else if (symbol_is_typename(d->id)) {
            printf("decl-type ");
      }

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

struct decl* decl_copy_deep(struct decl* d) {
      if (!d) return NULL;
      return decl_new(d->id,
            type_copy_deep(d->type), exp_copy_deep(d->exp),
            decls_copy_deep(d->decls), stmts_copy_deep(d->stmts));
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
      free(d);
}

void decls_destroy(GList* decls) {
      list_free_full(decls, (DestroyFunc)decl_destroy);
}

/*** Expressions. ***/

struct exp* exp_new(int class) {
      struct exp* this = calloc(1, sizeof(struct exp));
      this->class = class;
      return this;
}

struct exp* exp_binop_new(int class, struct exp* left, struct exp* right) {
      struct exp* this = exp_new(class);
      this->left = left;
      this->right = right;
      return this;
}

struct exp* exp_not_new(struct exp* right) {
      return exp_binop_new(AST_EXP_NOT, NULL, right);
}

struct exp* exp_num_new(int num) {
	struct exp* this = exp_new(AST_EXP_NUM);
	this->num = num;
	return this;
}

struct exp* exp_str_new(char* str) {
      struct exp* this = exp_new(AST_EXP_STR);
      this->str = str;
      return this;
}

struct exp* exp_id_new(Symbol id) {
      struct exp* this = exp_new(AST_EXP_ID);
      this->id = id;
      return this;
}

struct exp* exp_struct_lit_new(GList* field_inits) {
      struct exp* this = exp_new(AST_EXP_STRUCT_LIT);
      this->params = field_inits;
      return this;
}

struct exp* exp_array_lit_new(GList* exps) {
      struct exp* this = exp_new(AST_EXP_ARRAY_LIT);
      this->params = exps;
      return this;
}

struct exp* exp_fun_call_new(Symbol id, GList* params) {
      struct exp* this = exp_new(AST_EXP_FUN_CALL);
      this->id = id;
      this->params = params;
      return this;
}

struct exp* exp_array_index_new(struct exp* arr, struct exp* idx) {
      return exp_binop_new(AST_EXP_ARRAY_INDEX, arr, idx);
}

struct exp* exp_field_lookup_new(struct exp* strct, Symbol field) {
      struct exp* this = exp_new(AST_EXP_FIELD_LOOKUP);
      this->left = strct;
      this->id = field;
      return this;
}

static void binop_print(const char* op, struct exp* left, struct exp* right) {
      printf("%s ", op);
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
      switch (exp->class) {
            case AST_EXP_PLUS:
                  binop_print("+", exp->left, exp->right);
                  break;
            case AST_EXP_MINUS:
                  binop_print("-", exp->left, exp->right);
                  break;
            case AST_EXP_DIV:
                  binop_print("/", exp->left, exp->right);
                  break;
            case AST_EXP_MOD:
                  binop_print("%", exp->left, exp->right);
                  break;
            case AST_EXP_MUL:
                  binop_print("*", exp->left, exp->right);
                  break;

            case AST_EXP_OR:
                  binop_print("|", exp->left, exp->right);
                  break;
            case AST_EXP_AND:
                  binop_print("&", exp->left, exp->right);
                  break;
            case AST_EXP_NOT:
                  printf("! "); exp_print(exp->right);
                  break;
            case AST_EXP_LT:
                  binop_print("<", exp->left, exp->right);
                  break;
            case AST_EXP_LT_EQ:
                  binop_print("<=", exp->left, exp->right);
                  break;
            case AST_EXP_GT:
                  binop_print(">", exp->left, exp->right);
                  break;
            case AST_EXP_GT_EQ:
                  binop_print(">=", exp->left, exp->right);
                  break;

            case AST_EXP_EQ:
                  binop_print("==", exp->left, exp->right);
                  break;
            case AST_EXP_NOT_EQ:
                  binop_print("!=", exp->left, exp->right);
                  break;

            case AST_EXP_NUM:
                  printf("num %d", exp->num);
                  break;
            case AST_EXP_TRUE:
                  printf("true");
                  break;
            case AST_EXP_FALSE:
                  printf("false");
                  break;
            case AST_EXP_NIL:
                  printf("nil");
                  break;
            case AST_EXP_STR:
                  printf("str %s", exp->str);
                  break;
            case AST_EXP_ID:
                  printf("id %s", symbol_to_str(exp->id));
                  break;

            case AST_EXP_STRUCT_LIT:
                  printf("struct-lit ");
                  field_inits_print(exp->params);
                  break;

            case AST_EXP_ARRAY_LIT:
                  printf("array-lit ");
                  exps_print(exp->params);
                  break;

            case AST_EXP_FUN_CALL:
                  printf("fun-call ");
                  id_print(exp->id);
                  printf(" ");
                  exps_print(exp->params);
                  break;

            case AST_EXP_ARRAY_INDEX:
                  printf("array-idx ");
                  exp_print(exp->left);
                  printf(" ");
                  exp_print(exp->right);
                  break;
            case AST_EXP_FIELD_LOOKUP:
                  printf("field-lkup ");
                  exp_print(exp->left);
                  printf(" ");
                  id_print(exp->id);
      }
      printf(")");
}

void exps_print(GList* exps) {
      printf("(exps");
      if (exps) printf(" ");
      list_print_spaced(exps, (PrintFunc)exp_print);
      printf(")");
}

struct exp* exp_copy_deep(struct exp* exp) {
      if (!exp) return NULL;

      struct exp* this = exp_new(exp->class);

      this->num = exp->num;      
      this->id = exp->id;
      if (exp->str) this->str = strdup(exp->str);
      this->left = exp_copy_deep(exp->left);
      this->right = exp_copy_deep(exp->right);

      switch (exp->class) {
            case AST_EXP_STRUCT_LIT:
                  this->params = field_inits_copy_deep(exp->params);
                  break;
            case AST_EXP_ARRAY_LIT:
            case AST_EXP_FUN_CALL:
                  this->params = exps_copy_deep(exp->params);
                  break;
      }
      return this;
}

GList* exps_copy_deep(GList* exps) {
      return list_copy_deep(exps, (CopyFunc)exp_copy_deep, NULL);
}

void exp_destroy(struct exp* exp) {
      if (!exp) return;

      if (exp->str) free(exp->str);
      exp_destroy(exp->left);
      exp_destroy(exp->right);
      switch (exp->class) {
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
      struct field_init* this = calloc(1, sizeof(struct field_init));
      this->id = field;
      this->init = init;
      return this;
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
      list_print_spaced(field_inits, (PrintFunc)field_init_print);
      printf(")");
}

struct field_init* field_init_copy_deep(struct field_init* fi) {
      if (!fi) return NULL;
      struct field_init* this = field_init_new(fi->id, fi->init);
      this->init = exp_copy_deep(fi->init);
      return this;
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

static struct stmt* stmt_new(int class) {
      struct stmt* this = calloc(1, sizeof(struct stmt));
      this->class = class;
      return this;
}

struct stmt* stmt_exp_new(struct exp* exp) {
      struct stmt* this = stmt_new(AST_STMT_EXP);
      this->exp = exp;
      return this;
}

struct stmt* stmt_assign_new(struct exp* left, struct exp* right) {
      struct stmt* this = stmt_new(AST_STMT_ASSIGN);
      this->left = left;
      this->right = right;
      return this;
}

struct stmt* stmt_if_new(struct exp* cond, GList* then_block, GList* else_block) {
      struct stmt* this = stmt_new(AST_STMT_IF);
      this->exp = cond;
      this->block1 = then_block;
      this->block2 = else_block;
      return this;
}

struct stmt* stmt_while_new(struct exp* cond, GList* block) {
      struct stmt* this = stmt_new(AST_STMT_WHILE);
      this->exp = cond;
      this->block1 = block;
      return this;
}

struct stmt* stmt_for_new(struct exp* lval, struct exp* from, struct exp* to, GList* block) {
      struct stmt* this = stmt_new(AST_STMT_FOR);
      this->left = lval;
      this->exp = from;
      this->right = to;
      this->block1 = block;
      return this;
}

struct stmt* stmt_return_new(struct exp* exp) {
      struct stmt* this = stmt_new(AST_STMT_RETURN);
      this->exp = exp;
      return this;
}

void stmt_print(struct stmt* stmt) {
      assert(stmt);
      indent_enter();
      printf("(");
      switch (stmt->class) {
            case AST_STMT_EXP:
                  printf("exp-stmt");
                  if (stmt->exp) printf(" ");
                  exp_print(stmt->exp);
                  break;

            case AST_STMT_ASSIGN:
                  printf("assign ");
                  exp_print(stmt->left);
                  printf(" ");
                  exp_print(stmt->right);
                  break;

            case AST_STMT_IF:
                  printf("if ");
                  exp_print(stmt->exp);
                  stmts_print(stmt->block1);
                  stmts_print(stmt->block2);
                  break;

            case AST_STMT_WHILE:
                  printf("while ");
                  exp_print(stmt->exp);
                  stmts_print(stmt->block1);
                  break;

            case AST_STMT_FOR:
                  printf("for ");
                  exp_print(stmt->left);
                  printf(" ");
                  exp_print(stmt->exp);
                  printf(" ");
                  exp_print(stmt->right);
                  stmts_print(stmt->block1);
                  break;

            case AST_STMT_RETURN:
                  printf("return");
                  if (stmt->exp) {
                        printf(" ");
                        exp_print(stmt->exp);
                  }
                  break;
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

struct stmt* stmt_copy_deep(struct stmt* stmt) {
      if (!stmt) return NULL;
      struct stmt* this = stmt_new(stmt->class);
      this->left = exp_copy_deep(stmt->left);
      this->exp = exp_copy_deep(stmt->exp);
      this->right = exp_copy_deep(stmt->right);
      this->block1 = stmts_copy_deep(stmt->block1);
      this->block2 = stmts_copy_deep(stmt->block2);
      return this;
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

void list_print_spaced(GList* list, PrintFunc print) {
      list_print_with_sep(list, print, " ");
}

void list_print_commad(GList* list, PrintFunc print) {
      list_print_with_sep(list, print, ", ");
}

void list_print_with_sep(GList* list, PrintFunc print, const char* sep) {
      GList* itr;
      for (itr = g_list_first(list); itr != g_list_last(itr); itr = g_list_next(itr)) {
            print(itr->data);
            printf(sep);
      }
      if (itr) print(itr->data);
}



/////////////////////////TYPECHECKINGGGGGGGG/////////////////////

void decl_print_type(struct decl* d, Env* env) {
	indent_enter();
      printf("(");
      if (symbol_is_var(d->id)) {
            if(d->status == TYPE_NONE) {
                  printf("decl-var:NONE ");
            } else {
                  printf("decl-var:ok ");
            }
      } else if (symbol_is_fun(d->id)) {
		//check for main
            if(d->status == TYPE_NONE) {
                  printf("decl-fun:NONE ");
            } else {
                  printf("decl-fun:ok ");
            }
      } else if (symbol_is_typename(d->id)) {
            if(d->status == TYPE_NONE) {
                  printf("decl-type:NONE ");
            } else {
                  printf("decl-type:ok ");
            }
      }

      id_print_type(d->id, env);
      printf(" ");
      type_print(d->type);

      if (d->exp) {
            printf(" ");
            exp_print_type(d->exp, env);
      } else if (d->decls || d->stmts) {
            decls_print_type(d->decls, env);
            stmts_print_type(d->stmts, env);
      }
      printf(")");
      indent_exit();
}

void decls_print_type(GList* decls, Env* env) {
      indent_enter();
      printf("(decls");
      g_list_foreach(decls, (GFunc)decl_print_type, env);
      printf(")");
      indent_exit();
}

static void binop_print_type(const char* op, struct exp* left, struct exp* right, Env *env) {
      if(left->status == TYPE_INT && right->status == TYPE_INT) {
            printf("%s:int ", op);
      } else if(left->status == TYPE_BOOL && right->status == TYPE_BOOL) {
            printf("%s:bool ", op);
      } else {
            printf("%s ", op);
      }
      exp_print_type(left, env);
      printf(" ");
      exp_print_type(right, env);
}

void id_print_type(Symbol symbol, Env *env) {
      printf("(id %s)", symbol_to_str(symbol));
}

void exp_print_type(struct exp* exp, Env *env) {
      if (!exp) return;
      printf("(");
      switch (exp->class) {
            case AST_EXP_PLUS:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_INT;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("+", exp->left, exp->right,env);
                  break;
            case AST_EXP_MINUS:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_INT;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("-", exp->left, exp->right,env);
                  break;
            case AST_EXP_DIV:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_INT;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("/", exp->left, exp->right,env);
                  break;
            case AST_EXP_MOD:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_INT;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("%", exp->left, exp->right,env);
                  break;
            case AST_EXP_MUL:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_INT;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("*", exp->left, exp->right,env);
                  break;
            case AST_EXP_OR:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("|", exp->left, exp->right,env);
                  break;
            case AST_EXP_AND:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("&", exp->left, exp->right,env);
                  break;
            case AST_EXP_NOT:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  printf("! "); exp_print_type(exp->right,env);
                  break;
            case AST_EXP_LT:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("<", exp->left, exp->right,env);
                  break;
            case AST_EXP_LT_EQ:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("<=", exp->left, exp->right,env);
                  break;
            case AST_EXP_GT:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type(">", exp->left, exp->right,env);
                  break;
            case AST_EXP_GT_EQ:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type(">=", exp->left, exp->right,env);
                  break;

            case AST_EXP_EQ:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("==", exp->left, exp->right,env);
                  break;
            case AST_EXP_NOT_EQ:
                  if(TYPE_INT == exp->left->status && TYPE_INT == exp->right->status) {
                        exp->status = TYPE_BOOL;
                        exp->left->status = TYPE_BOOL;
                        exp->right->status = TYPE_BOOL;
                  } else {
                        exp->status = TYPE_NONE;
                  }
                  binop_print_type("!=", exp->left, exp->right,env);
                  break;

            case AST_EXP_NUM:
                  exp->status = TYPE_INT;
                  printf("num:%s %d","int", exp->num);
                  break;
            case AST_EXP_TRUE:
                  printf("true");
                  break;
            case AST_EXP_FALSE:
                  printf("false");
                  break;
            case AST_EXP_NIL:
                  printf("nil");
                  break;
            case AST_EXP_STR:
                  printf("str %s", exp->str);
                  break;
            case AST_EXP_ID:
                  printf("id %s", symbol_to_str(exp->id));
                  break;

            case AST_EXP_STRUCT_LIT:
                  printf("struct-lit ");
                  field_inits_print_type(exp->params,env);
                  break;

            case AST_EXP_ARRAY_LIT:
                  printf("array-lit ");
                  exps_print_type(exp->params,env);
                  break;

            case AST_EXP_FUN_CALL:
                  printf("fun-call ");
                  id_print_type(exp->id,env);
                  printf(" ");
                  exps_print_type(exp->params,env);
                  break;

            case AST_EXP_ARRAY_INDEX:
                  printf("array-idx ");
                  exp_print_type(exp->left,env);
                  printf(" ");
                  exp_print_type(exp->right,env);
                  break;
            case AST_EXP_FIELD_LOOKUP:
                  printf("field-lkup ");
                  exp_print_type(exp->left,env);
                  printf(" ");
                  id_print_type(exp->id,env);
      }
      printf(")");
}

void exps_print_type(GList* exps,Env *env) {
      printf("(exps");
      if (exps) printf(" ");
      list_print_spaced_type(exps, (PrintFunc)exp_print_type, env);
      printf(")");
}

void field_init_print_type(struct field_init* fi,Env *env) {
      printf("(field-init ");
      id_print_type(fi->id, env);
      printf(" ");
      exp_print_type(fi->init,env);
      printf(")");
}

void field_inits_print_type(GList* field_inits,Env *env) {
      printf("(field-inits ");
      list_print_spaced_type(field_inits, (PrintFunc)field_init_print_type, env);
      printf(")");
}

void stmt_print_type(struct stmt* stmt,Env *env) {
      assert(stmt);
      indent_enter();
      printf("(");
      switch (stmt->class) {
            case AST_STMT_EXP:
                  printf("exp-stmt");
                  if (stmt->exp) printf(" ");
                  exp_print_type(stmt->exp,env);
                  break;

            case AST_STMT_ASSIGN:
                  printf("assign ");
                  exp_print_type(stmt->left,env);
                  printf(" ");
                  exp_print_type(stmt->right,env);
                  break;

            case AST_STMT_IF:
                  printf("if ");
                  exp_print_type(stmt->exp,env);
                  stmts_print_type(stmt->block1,env);
                  stmts_print_type(stmt->block2,env);
                  break;

            case AST_STMT_WHILE:
                  printf("while ");
                  exp_print_type(stmt->exp,env);
                  stmts_print_type(stmt->block1,env);
                  break;

            case AST_STMT_FOR:
                  printf("for ");
                  exp_print_type(stmt->left,env);
                  printf(" ");
                  exp_print_type(stmt->exp,env);
                  printf(" ");
                  exp_print_type(stmt->right,env);
                  stmts_print_type(stmt->block1,env);
                  break;

            case AST_STMT_RETURN:
                  printf("return");
                  if (stmt->exp) {
                        printf(" ");
                        exp_print_type(stmt->exp,env);
                  }
                  break;
      }
      printf(")");
      indent_exit();
}

void stmts_print_type(GList* stmts,Env *env) {
      indent_enter();
      printf("(stmts");
      g_list_foreach(stmts, (GFunc)stmt_print_type, NULL);
      printf(")");
      indent_exit();
}

void list_print_spaced_type(GList* list, PrintFunc print,Env *env) {
      list_print_with_sep_type(list, print, " ", env);
}

void list_print_commad_type(GList* list, PrintFunc print,Env *env) {
      list_print_with_sep_type(list, print, ", ", env);
}

void list_print_with_sep_type(GList* list, PrintFunc print, const char* sep,Env *env) {
      GList* itr;
      for (itr = g_list_first(list); itr != g_list_last(itr); itr = g_list_next(itr)) {
            print(itr->data);
            printf(sep);
      }
      if (itr) print(itr->data);
}
