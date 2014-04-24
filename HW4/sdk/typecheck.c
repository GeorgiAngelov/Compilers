#include "typecheck.h"
#include "type.h"
#include "ast.h"
#include "env.h"
#include <glib.h>
#include <assert.h>
#include <stdio.h>

static void annotate_stmt(struct stmt*, Env*);
static void annotate_decl(struct decl*, Env*);
static int stmts_all_ok(GList* stmts);

static const Type* annotate_exp(struct exp*, Env*);
static Type* annotate_array_lit(GList*, Env*);
static Type* annotate_struct_lit(GList*, Env*);
static Type* annotate_fun_call(Symbol id, GList* params, Env* env);
static GList* annotate_exps(GList*, Env*);

static int subtype(const Type*, const Type*, Env* env);
static int params_subtype(GList*, GList*, Env* env);
static int decl_subtype(const struct decl*, const struct decl*, Env* env);
static int fields_subtype(GList*, GList*, Env* env);

static Type* supremum(const Type* left, const Type* right, Env* env);
static struct decl* decl_supremum(const struct decl* left, const struct decl* right, Env* env);
static GList* decls_supremum(GList* left, GList* right, Env* env);

void annotate_decls(GList* decls, Env* env) {
      g_list_foreach(decls, (GFunc)annotate_decl, env);
}

int decls_all_ok(GList* decls) {
      GList* itr;
      int ok = 1;

      for (itr = g_list_first(decls); itr; itr = g_list_next(itr)) {
            ok = ok && type_is_ok(((struct decl*)itr->data)->node_type);
      }
      return ok;
}

static void annotate_decl(struct decl* d, Env* env) {
      assert(env);
      assert(d);
      assert(!d->node_type || type_is_conflict(d->node_type));

      Type* calculated = NULL;

      if (d->exp) {
            const Type* t = annotate_exp(d->exp, env);
            if (subtype(t, d->type, env)) {
                  calculated = type_ok_new();
            }
      } else if (d->decls || d->stmts) {
            Env* fun_env = env_lookup_fun_env(env, d->id);
            assert(fun_env);

            Env* merged_env = env_union(env, fun_env);
            assert(merged_env);

            g_list_foreach(d->decls, (GFunc)annotate_decl, merged_env);
            g_list_foreach(d->stmts, (GFunc)annotate_stmt, merged_env);
            env_destroy(merged_env);

            if (decls_all_ok(d->decls) && stmts_all_ok(d->stmts)) {
                  calculated = type_ok_new();
            }
      } else {
            calculated = type_ok_new();
      }

      if (type_is_conflict(d->node_type)) {
            type_destroy(d->node_type);
            type_destroy(calculated);
            d->node_type = NULL;
      } else {
            d->node_type = calculated;
      }
}

static void annotate_stmt(struct stmt* stmt, Env* env) {
      assert(env);

      stmt->node_type = NULL;

      switch (stmt->kind) {
            case AST_STMT_EXP: {
                  assert(stmt->exp);

                  if (annotate_exp(stmt->exp, env)) {
                        stmt->node_type = type_ok_new();
                  }

                  break;
            }

            case AST_STMT_ASSIGN: {
                  assert(stmt->left);
                  assert(stmt->right);

                  const Type* left = annotate_exp(stmt->left, env);
                  const Type* right = annotate_exp(stmt->right, env);

                  if (subtype(right, left, env)) {
                        stmt->node_type = type_ok_new();
                  }

                  break;
            }

            case AST_STMT_IF: {
                  assert(stmt->exp);

                  const Type* cond = annotate_exp(stmt->exp, env);

                  g_list_foreach(stmt->block1, (GFunc)annotate_stmt, env);
                  g_list_foreach(stmt->block2, (GFunc)annotate_stmt, env);

                  if (type_is_bool(cond) && stmts_all_ok(stmt->block1) && stmts_all_ok(stmt->block2)) {
                        stmt->node_type = type_ok_new();
                  }

                  break;
            }

            case AST_STMT_WHILE: {
                  assert(stmt->exp);

                  const Type* cond = annotate_exp(stmt->exp, env);
                  g_list_foreach(stmt->block1, (GFunc)annotate_stmt, env);

                  if (type_is_bool(cond) && stmts_all_ok(stmt->block1)) {
                        stmt->node_type = type_ok_new();
                  }

                  break;
            }

            case AST_STMT_FOR: {
                  assert(stmt->exp);
                  assert(stmt->left);
                  assert(stmt->right);

                  const Type* lval = annotate_exp(stmt->left, env);
                  const Type* from = annotate_exp(stmt->exp, env);
                  const Type* to = annotate_exp(stmt->right, env);

                  g_list_foreach(stmt->block1, (GFunc)annotate_stmt, env);

                  if (type_is_int(lval)
                        && type_is_int(from)
                        && type_is_int(to)
                        && stmts_all_ok(stmt->block1)) {
                        stmt->node_type = type_ok_new();
                  }

                  break;
            }

            case AST_STMT_RETURN: {
                  Type* expected = env_lookup(env, symbol_fun_return());

                  Type* actual = NULL;
                  if (stmt->exp) {
                        actual = type_copy_deep(annotate_exp(stmt->exp, env));
                  } else {
                        actual = type_void_new();
                  }

                  if (subtype(actual, expected, env)) {
                        stmt->node_type = type_ok_new();
                  }

                  type_destroy(actual);
                  break;
            }
      }
}

static int stmts_all_ok(GList* stmts) {
      GList* itr;
      int ok = 1;

      for (itr = g_list_first(stmts); itr; itr = g_list_next(itr)) {
            ok = ok && ((struct stmt*)itr->data)->node_type;
      }
      return ok;
}

static Type* annotate_struct_lit(GList* inits, Env* env) {
      assert(inits);
      assert(env);
      
      GList* decls = NULL;

      for (inits = g_list_first(inits); inits; inits = g_list_next(inits)) {
            assert(inits->data);
            Symbol id = ((struct field_init*)inits->data)->id;
            struct exp* init = ((struct field_init*)inits->data)->init;
            assert(init);

            const Type* t = annotate_exp(init, env);

            decls = g_list_append(decls, decl_field_new(symbol_to_str(id), type_copy_deep(t)));
      }

      return type_struct_new(decls);
}

static Type* annotate_array_lit(GList* inits, Env* env) {
      assert(inits);
      assert(env);

      GList* itr = g_list_first(inits);

      assert(itr);

      Type* sup = type_copy_deep(annotate_exp((struct exp*)itr->data, env));

      for (itr = g_list_next(itr); itr; itr = g_list_next(itr)) {
            Type* temp = sup;
            const Type* next_type = annotate_exp((struct exp*)itr->data, env);

            sup = supremum(sup, next_type, env);

            type_destroy(temp);
      }

      return type_array_new(sup);
}

static Type* annotate_fun_call(Symbol id, GList* params, Env* env) {
      Type* fun = env_lookup(env, id);
      GList* call_params = annotate_exps(params, env);
      GList* decl_params = NULL;
      Type* node_type = NULL;

      if (fun) {
            GList* decl_params = decls_to_types(type_fun_get_params(fun));

            assert(type_fun_get_ret(fun));

            if (params_subtype(call_params, decl_params, env)) {
                  if (symbol_equal(symbol_fun("resize"), id)) {
                        assert(call_params);

                        Type* first_param = (Type*)g_list_first(call_params)->data;

                        if (!type_is_nil(first_param)) {
                              node_type = type_copy_deep(first_param);
                        }
                  } else {
                        node_type = type_copy_deep(type_fun_get_ret(fun));
                  }
            }
      }

      g_list_free(decl_params);
      g_list_free(call_params);
      return node_type;
}

static const Type* annotate_exp(struct exp* exp, Env* env) {
      assert(exp);

      exp->node_type = NULL;

      switch (exp->kind) {
            case AST_EXP_PLUS:
            case AST_EXP_MINUS:
            case AST_EXP_DIV:
            case AST_EXP_MOD:
            case AST_EXP_MUL: {
                  assert(exp->left);
                  assert(exp->right);

                  const Type* left = annotate_exp(exp->left, env);
                  const Type* right = annotate_exp(exp->right, env);

                  if (type_is_int(left) && type_is_int(right)) {
                        exp->node_type = type_int_new();
                  }

                  break;
            }

            case AST_EXP_OR:
            case AST_EXP_AND: {
                  assert(exp->left);
                  assert(exp->right);

                  const Type* left = annotate_exp(exp->left, env);
                  const Type* right = annotate_exp(exp->right, env);

                  if (type_is_bool(left) && type_is_bool(right)) {
                        exp->node_type =  type_bool_new();
                  }

                  break;
            }

            case AST_EXP_NOT: {
                  assert(exp->right);

                  const Type* right = annotate_exp(exp->right, env);

                  if (type_is_bool(right)) {
                        exp->node_type = type_bool_new();
                  }

                  break;
            }

            case AST_EXP_LT:
            case AST_EXP_LT_EQ:
            case AST_EXP_GT:
            case AST_EXP_GT_EQ: {
                  assert(exp->left);
                  assert(exp->right);

                  const Type* left = annotate_exp(exp->left, env);
                  const Type* right = annotate_exp(exp->right, env);

                  if (type_is_int(left) && type_is_int(right)) {
                        exp->node_type = type_bool_new();
                  }

                  break;
            }

            case AST_EXP_EQ:
            case AST_EXP_NOT_EQ: {
                  assert(exp->left);
                  assert(exp->right);

                  const Type* left = annotate_exp(exp->left, env);
                  const Type* right = annotate_exp(exp->right, env);

                  if (type_equal(left, right) || (type_is_obj(left) && type_is_obj(right))) {
                        exp->node_type = type_bool_new();
                  }

                  break;
            }

            case AST_EXP_NUM: {
                  exp->node_type = type_int_new();
                  break;
            }

            case AST_EXP_TRUE: {
                  exp->node_type = type_bool_new();
                  break;
            }

            case AST_EXP_FALSE: {
                  exp->node_type = type_bool_new();
                  break;
            }

            case AST_EXP_NIL: {
                  exp->node_type = type_nil_new();
                  break;
            }

            case AST_EXP_STR: {
                  exp->node_type = type_array_new(type_int_new());
                  break;
            }

            case AST_EXP_ID: {
                  assert(symbol_is_var(exp->id));
                  exp->node_type = type_copy_deep(env_lookup(env, exp->id));
                  break;
            }

            case AST_EXP_STRUCT_LIT: {
                  assert(exp->params);
                  exp->node_type = annotate_struct_lit(exp->params, env);
                  break;
            }

            case AST_EXP_ARRAY_LIT: {
                  assert(exp->params);
                  exp->node_type = annotate_array_lit(exp->params, env);
                  break;
            }

            case AST_EXP_FUN_CALL: {
                  assert(symbol_is_fun(exp->id));
                  exp->node_type = annotate_fun_call(exp->id, exp->params, env);
                  break;
            }

            case AST_EXP_ARRAY_INDEX: {
                  assert(exp->left);
                  assert(exp->right);

                  const Type* arr = type_expand(annotate_exp(exp->left, env), env);
                  const Type* idx = annotate_exp(exp->right, env);

                  if (type_is_array(arr) && type_is_int(idx)) {
                        exp->node_type = type_copy_deep(type_array_get_eles(arr));
                  } 

                  break;
            }

            case AST_EXP_FIELD_LOOKUP: {
                  assert(exp->left);
                  assert(symbol_is_field(exp->id));

                  const Type* lval = type_expand(annotate_exp(exp->left, env), env);

                  if (type_is_struct(lval)) {
                        exp->node_type = type_copy_deep(type_struct_get_field(lval, exp->id));
                  }

                  break;
            }
      }
      return exp->node_type;
}

// A list of the node_types after annotation. Doesn't copy the types; use
// g_list_free().
static GList* annotate_exps(GList* exps, Env* env) {
      return list_copy_deep(exps, (CopyFunc)annotate_exp, env);
}

/*** Subtyping, supremum. ***/

static int subtype(const Type* left, const Type* right, Env* env) {
      if (type_equal(left, right)) return 1;

      if (!left) return 0; // no type is a subtype of no type but no type.
      if (!right) return 1; // every type is a subtype of no type.

      right = type_expand(right, env);

      if (type_equal(left, right)) return 1;

      switch (right->kind) {
            case TYPE_ARRAY:
                  return type_is_nil(left)
                        || (type_is_array(left) && subtype(left->eles, right->eles, env));

            case TYPE_STRUCT:
                  return type_is_nil(left)
                        || (type_is_struct(left) && fields_subtype(left->fields, right->fields, env));

            default: return 0;
      }
}

// For typechecking function call parameters. Runs subtype() pairwise
// on (0,0) then (1,1), etc.
static int params_subtype(GList* left, GList* right, Env* env) {
      if (left == right) return 1;
      if (!left || !right) return 0;
      if (g_list_length(left) != g_list_length(right))  return 0;

      left = g_list_first(left);
      right = g_list_first(right);
      while (right) {
            assert(right->data);

            if (!left->data) return 0;
            if (!subtype((Type*)left->data, (Type*)right->data, env))
                  return 0;

            left = g_list_next(left);
            right = g_list_next(right);
      }
      return 1;
}

static int decl_subtype(const struct decl* left, const struct decl* right, Env* env) {
      assert(left);
      assert(right);

      return symbol_equal(left->id, right->id) && subtype(left->type, right->type, env);
}

static int fields_subtype(GList* left, GList* right, Env* env) {
      assert(env);
      if (left == right) return 1;
      if (g_list_length(left) != g_list_length(right)) return 0;

      // We can assume both lists are sorted.
      left = g_list_first(left);
      right = g_list_first(right);

      while (right) {
            assert(left->data);
            assert(right->data);

            if (!decl_subtype((struct decl*)left->data, (struct decl*)right->data, env)) {
                  return 0;
            }

            left = g_list_next(left);
            right = g_list_next(right);
      }
      return 1;
}

// Calculates the supremum of two (calculated) types. Creates a new allocation
// as basically a deep copy of one of the parameter types. Use type_destroy()
// to free it.
static Type* supremum(const Type* left, const Type* right, Env* env) {
      if (!left || !right) return NULL;

      if (type_equal(left, right)) return type_copy_deep(left);

      if (type_is_id(left)) {
            if (subtype(right, left, env)) return type_copy_deep(left);
            else return NULL;
      }

      if (type_is_id(right)) {
            if (subtype(left, right, env)) return type_copy_deep(right);
            else return NULL;
      }

      if (type_is_nil(left) && type_is_obj(right))
            return type_copy_deep(right);

      if (type_is_nil(right) && type_is_obj(left))
            return type_copy_deep(left);

      if (type_is_array(left) && type_is_array(right)) {
            return type_array_new(supremum(left->eles, right->eles, env));
      }

      if (type_is_struct(left) && type_is_struct(right)) {
            GList* fields = decls_supremum(left->fields, right->fields, env);
            if (fields) return type_struct_new(fields);
            else return NULL;
      }

      return NULL;
}

static struct decl* decl_supremum(const struct decl* left, const struct decl* right, Env* env) {
      if (!left || !right) return NULL;

      if (!symbol_equal(left->id, right->id)) return NULL;

      Type* sup = supremum(left->type, right->type, env);

      if (type_equal(sup, left->type)) {
            type_destroy(sup);
            return decl_copy_deep(left);
      }
      else if (type_equal(sup, right->type)) {
            type_destroy(sup);
            return decl_copy_deep(right);
      }

      return NULL;
}

static GList* decls_supremum(GList* left, GList* right, Env* env) {
      if (!left || !right) return NULL;
      if (g_list_length(left) != g_list_length(right))  return NULL;

      GList* sup = NULL;

      left = g_list_first(left);
      right = g_list_first(right);
      while (left) {
            assert(left->data);
            assert(right->data);

            struct decl* field = decl_supremum((struct decl*)left->data, (struct decl*)right->data, env);
            if (!field) {
                  g_list_free(sup);
                  return NULL;
            }

            sup = g_list_append(sup, field);

            left = g_list_next(left);
            right = g_list_next(right);
      }
      return sup;
}


