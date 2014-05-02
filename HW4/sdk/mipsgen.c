#include "typecheck.h"
#include "mipsgen.h"
#include "type.h"
#include "ast.h"
#include "env.h"
#include <glib.h>
#include <assert.h>
#include <stdio.h>
FILE *out;

extern int count;

//local declarations
void mips_generate_text(FILE* out_ptr, GList * decls, Env* env){
	//printf("Inside mips_generate_text\n");
	out = out_ptr;
	g_list_foreach(decls, (GFunc)mips_traverse_decl, env);
}

static void mips_traverse_decl(struct decl* d, Env* env) {
	//printf("Inside mips_traverse_decl\n");
	Type* calculated = NULL;

	//if the declaration has any expressions
	if (d->exp) {
		const Type* t = mips_traverse_exp(d->exp, env);
	}
	//if the declaration has any declarations or statements(hence it is a function)
	else if (d->decls || d->stmts) {
		Env* fun_env = env_lookup_fun_env(env, d->id);

		Env* merged_env = env_union(env, fun_env);

		g_list_foreach(d->decls, (GFunc)mips_traverse_decl, merged_env);
		g_list_foreach(d->stmts, (GFunc)mips_traverse_stmt, merged_env);
		env_destroy(merged_env);
	}
}

void print_exp_type (int kind)
{
	switch (kind)
	{
		case AST_EXP_PLUS:
		{
			printf("add");
			break;
		}
		case AST_EXP_MINUS:
		{
			printf("sub");
			break;
		}
		case AST_EXP_DIV:
		{
			printf("div");
			break;
		}
		case AST_EXP_MOD:
		{
			printf("mod");
			break;
		}
		case AST_EXP_MUL:
		{
			printf("mul");
			break;
		}
		case AST_EXP_OR:
		{
			printf("or");
			break;
		}
	}
	
	printf(" ");
	
}

static const Type* mips_traverse_exp(struct exp* exp, Env* env) {
	//printf("Inside mips_traverse_exp\n");
	exp->node_type = NULL;

	switch (exp->kind) {
		case AST_EXP_PLUS:
		case AST_EXP_MINUS:
		case AST_EXP_DIV:
		case AST_EXP_MOD:
		case AST_EXP_MUL: {
			//const Type* left = mips_traverse_exp(exp->left, env);
			//const Type* right = mips_traverse_exp(exp->right, env);
			int leftC;
			int rightC;
			//if both expressions are only nums, then we store them in a register
			if (exp->left->kind == AST_EXP_NUM && exp->right->kind == AST_EXP_NUM) {
				//printf("li $t0, %d\n", exp->left->num);
				//printf("li $t1, %d\n", exp->right->num);
				
				
				printf("store %d in t%d\n", exp->left->num, count);
				leftC = count;
				count++;
				printf("store %d in t%d\n", exp->right->num, count);
				rightC = count;
				count++;
				print_exp_type(exp->kind);
				printf(" $v0, $t%d, $t%d\n", leftC, rightC);
				count = count -2;
			}
			//if the LEFT one is the only num, then store it in a register, because the right side is already in a register
			else if(exp->left->kind == AST_EXP_NUM){
				
				
				//printf("li $t0, %d\n", exp->left->num);
				
				
				printf("store %d in t%d\n", exp->left->num, count);
				leftC = count;
				count++;
				 mips_traverse_exp(exp->right, env);
				 //result of right is in v0
				 printf("move v0 to t%d\n", count);
				 rightC = count;
				 count++;
				 print_exp_type(exp->kind);
				printf(" $v0, $t%d, $t%d\n", leftC, rightC);
				count = count -2;
			}
			//if the RIGHT one is the only num, then store it in a register, because the right side is already in a register
			else if(exp->right->kind == AST_EXP_NUM){
				//printf("li $t1, %d\n", exp->right->num);
				
				
				
				printf("store %d in t%d\n", exp->right->num, count);
				rightC = count;
				count++;
				mips_traverse_exp(exp->left, env);
				//result of left is in v0
				printf("move v0 to t%d\n", count);
				leftC = count;
				count++;
				print_exp_type(exp->kind);
				printf(" $v0, $t%d, $t%d\n", leftC, rightC);
				count = count -2;
				
				
			}
			//if neither one is a NUM, it means that we stored them in a registerr, LEFT Is always $2, and RIGHT Is always $3
			else{
				
				mips_traverse_exp(exp->left, env);
				//result of left is now stored v0
				printf("move v0 in t%d\n", count);
				leftC = count;
				count++;
				mips_traverse_exp(exp->right, env);
				//result of right is now stored v0
				printf("move v0 in t%d\n", count);
				rightC = count;
				count++;
				print_exp_type(exp->kind);
				printf(" $v0, $t%d, $t%d\n", leftC, rightC);
				count = count -2;
			}
			exp->node_type = type_int_new();
			break;
		}

		case AST_EXP_OR:
		case AST_EXP_AND: {
			  const Type* left = mips_traverse_exp(exp->left, env);
			  const Type* right = mips_traverse_exp(exp->right, env);

			  if (type_is_bool(left) && type_is_bool(right)) {
					exp->node_type =  type_bool_new();
			  }

			  break;
		}

		case AST_EXP_NOT: {
			  const Type* right = mips_traverse_exp(exp->right, env);

			  if (type_is_bool(right)) {
					exp->node_type = type_bool_new();
			  }

			  break;
		}

		case AST_EXP_LT:
		case AST_EXP_LT_EQ:
		case AST_EXP_GT:
		case AST_EXP_GT_EQ: {
			  const Type* left = mips_traverse_exp(exp->left, env);
			  const Type* right = mips_traverse_exp(exp->right, env);

			  if (type_is_int(left) && type_is_int(right)) {
					exp->node_type = type_bool_new();
			  }

			  break;
		}

		case AST_EXP_EQ:
		case AST_EXP_NOT_EQ: {
			const Type* left = mips_traverse_exp(exp->left, env);
			const Type* right = mips_traverse_exp(exp->right, env);

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
			exp->node_type = type_copy_deep(env_lookup(env, exp->id));
			break;
		}

		case AST_EXP_STRUCT_LIT: {
			  /*assert(exp->params);
			  exp->node_type = annotate_struct_lit(exp->params, env);*/
			  break;
		}

		case AST_EXP_ARRAY_LIT: {
			/*assert(exp->params);
			exp->node_type = annotate_array_lit(exp->params, env);*/
			break;
		}

		case AST_EXP_FUN_CALL: {
			/*assert(symbol_is_fun(exp->id));
			exp->node_type = annotate_fun_call(exp->id, exp->params, env);*/
			break;
		}

		case AST_EXP_ARRAY_INDEX: {
			const Type* arr = type_expand(mips_traverse_exp(exp->left, env), env);
			const Type* idx = mips_traverse_exp(exp->right, env);

			if (type_is_array(arr) && type_is_int(idx)) {
				exp->node_type = type_copy_deep(type_array_get_eles(arr));
			}

			break;
		}

		case AST_EXP_FIELD_LOOKUP: {
			/*assert(exp->left);
			assert(symbol_is_field(exp->id));

			const Type* lval = type_expand(mips_traverse_exp(exp->left, env), env);

			if (type_is_struct(lval)) {
				exp->node_type = type_copy_deep(type_struct_get_field(lval, exp->id));
			}*/
			break;
		}
	}
	return exp->node_type;
}

static const void mips_traverse_stmt(struct stmt* stmt, Env* env){
	stmt->node_type = NULL;

	switch (stmt->kind) {
		case AST_STMT_EXP: {
			  if (mips_traverse_exp(stmt->exp, env)) {
					stmt->node_type = type_ok_new();
			  }
			  break;
		}
		case AST_STMT_ASSIGN: {
			  const Type* left = mips_traverse_exp(stmt->left, env);
			  const Type* right = mips_traverse_exp(stmt->right, env);
			  break;
		}

		case AST_STMT_IF: {
			  const Type* cond = mips_traverse_exp(stmt->exp, env);

			  g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);
			  g_list_foreach(stmt->block2, (GFunc)mips_traverse_stmt, env);

			  break;
		}

		case AST_STMT_WHILE: {
			  const Type* cond = mips_traverse_exp(stmt->exp, env);
			  g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);

			  break;
		}

		case AST_STMT_FOR: {
			  const Type* lval = mips_traverse_exp(stmt->left, env);
			  const Type* from = mips_traverse_exp(stmt->exp, env);
			  const Type* to = mips_traverse_exp(stmt->right, env);

			  g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);

			  break;
		}

		case AST_STMT_RETURN: {
			  Type* expected = env_lookup(env, symbol_fun_return());

			  Type* actual = NULL;
			  if (stmt->exp) {
					actual = type_copy_deep(mips_traverse_exp(stmt->exp, env));
			  } else {
					actual = type_void_new();
			  }

			  type_destroy(actual);
			  break;
		}
	}
}


/*
void mips_find_print_main(Env* genv) {
	GList* params = NULL;
	params = g_list_append(params, decl_param_new("args", type_array_new(type_array_new(type_int_new()))));
	Type* maintype = type_fun_new(params, type_int_new());

	int main_defined = type_equal(maintype, env_lookup(genv, symbol_fun("main")));
	
	if(main_defined == 1){
		fprintf(out,"Main declared");
		return;
	}
	return;
}

void mips_find_print_declarations_local(Env* genv){
	GList *params = NULL;
	Type* inttype = type_int_new();	
	//int int_defined = type_equal(inttype, env_lookup(genv,
	return;
}

void mips_print_main(GList * ast_root){
	Env* genv = env_new();
	insert_decls(ast_root, genv);
	//decls_print(ast_root);
	g_list_foreach(genv, (GFunc)mips_find_print_main, NULL);
}*/
