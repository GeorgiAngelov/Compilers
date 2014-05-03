#include "typecheck.h"
#include "mipsgen.h"
#include "ast.h"
#include "env.h"
#include "frontend.h"

#include "lexer.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <glib.h>

#include <string>
#include <sstream>
#include <map>


enum {
     SYMBOL_INVALID = 256,
     SYMBOL_FIELD,
     SYMBOL_FUN,
     SYMBOL_TYPENAME,
     SYMBOL_VAR,
     SYMBOL_PSEUDO,
};

extern int count;
extern int label_count;
//extern FILE *out;
extern std::ofstream out;
extern std::map<std::string, int> local_variables;

int stack_count = 0;

std::string mips_label_gen() {
	std::string label = "_lbl";

	std::string s;
	std::stringstream out;
	out << label_count;
	s = out.str();

	label.append(s);  
	label_count++;

	return label;
}

//local declarations
void mips_generate_text(GList * decls, Env* env){
	//printf("Inside mips_generate_text\n");
	//out = out_ptr;
	g_list_foreach(decls, (GFunc)mips_traverse_decl, env);
}

static void mips_traverse_decl(struct decl* d, Env* env) {
	//printf("Inside mips_traverse_decl\n");
	Type* calculated = NULL;
	/*
	if(d->id.kind == SYMBOL_VAR){
	printf("var %s\n", symbol_to_str(d->id));
	}
	*/
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
	else
	{
		
		out << "sub $sp, $sp, 4" << std::endl;
		//set up var access in map
		std::string id(symbol_to_str((d->id)));
		stack_count += 4;
		local_variables[id] = stack_count;
		
	}
}

void print_exp_type (int kind)
{
	switch (kind)
	{
		case AST_EXP_PLUS:
		{
			//fprintf(out, "add");
			out << "add";
			break;
		}
		case AST_EXP_MINUS:
		{
			//fprintf(out, "sub");
			out << "sub";
			break;
		}
		case AST_EXP_DIV:
		{
			//fprintf(out, "div");
			out << "div";
			break;
		}
		case AST_EXP_MOD:
		{
			//fprintf(out, "rem");
			out << "rem";
			break;
		}
		case AST_EXP_MUL:
		{
			//fprintf(out, "mul");
			out << "mul";
			break;
		}
		case AST_EXP_AND:
		{
			//fprintf(out, "and");
			out << "and";
			break;
		}
		case AST_EXP_OR:
		{
			//fprintf(out, "or");
			out << "or";
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
		case AST_EXP_MUL: 
		case AST_EXP_OR:
		case AST_EXP_AND: {
			//const Type* left = mips_traverse_exp(exp->left, env);
			//const Type* right = mips_traverse_exp(exp->right, env);
			int leftC;
			int rightC;
			
			//if both expressions are only nums, then we store them in a register
			if 	(
				(exp->left->kind == AST_EXP_NUM && exp->right->kind == AST_EXP_NUM) || 
				(exp->left->kind == AST_EXP_TRUE && exp->right->kind == AST_EXP_FALSE) ||
				(exp->left->kind == AST_EXP_FALSE && exp->right->kind == AST_EXP_TRUE) ||
				(exp->left->kind == AST_EXP_TRUE && exp->right->kind == AST_EXP_TRUE) ||
				(exp->left->kind == AST_EXP_FALSE && exp->right->kind == AST_EXP_FALSE)
				) {
				//printf("li $t0, %d\n", exp->left->num);
				//printf("li $t1, %d\n", exp->right->num);
				//printf("case1\n");
				
				//fprintf(out, "li $t%d, %d\n", count, exp->left->num);
				out << "li $t" << count << ", " << exp->left->num << std::endl;
				leftC = count;
				count++;
				//fprintf(out, "li $t%d, %d\n", count, exp->right->num);
				out << "li $t" << count << ", " << exp->right->num << std::endl;
				rightC = count;
				count++;
			}
			//if the LEFT one is the only num, then store it in a register, because the right side is already in a register
			else if(	(exp->left->kind == AST_EXP_NUM) ||
					(exp->left->kind == AST_EXP_TRUE) ||
					(exp->left->kind == AST_EXP_FALSE)
					){
				//printf("case2\n");
				
				//printf("li $t0, %d\n", exp->left->num);
				
				
				//fprintf(out, "li $t%d, %d\n", count, exp->left->num);
				out << "li $t" << count << ", " << exp->left->num << std::endl;
				leftC = count;
				count++;
				 mips_traverse_exp(exp->right, env);
				 //result of right is in v0
				 //fprintf(out, "move $t%d, $v0\n", count);
				 out << "move $t" << count << ", $v0" << std::endl;
				 rightC = count;
				 count++;
			}
			//if the RIGHT one is the only num, then store it in a register, because the right side is already in a register
			else if(	(exp->right->kind == AST_EXP_NUM) ||
					(exp->right->kind == AST_EXP_TRUE) ||
					(exp->right->kind == AST_EXP_FALSE)
					){
				//printf("li $t1, %d\n", exp->right->num);
				
				//printf("case3\n");
				
				//fprintf(out, "li $t%d, %d\n", count, exp->right->num);
				out << "li $t" << count << ", " << exp->right->num << std::endl;
				rightC = count;
				count++;
				mips_traverse_exp(exp->left, env);
				//result of left is in v0
				//fprintf(out, "move $t%d, $v0\n", count);
				out << "move $t" << count << ", $v0" << std::endl;
				leftC = count;
				count++;
			}
			//if neither one is a NUM, it means that we stored them in a register, LEFT Is always $2, and RIGHT Is always $3
			else{
				//printf("case4\n");
				mips_traverse_exp(exp->left, env);
				//result of left is now stored v0
				//fprintf(out, "move $t%d, $v0\n", count);
				out << "move $t" << count << ", $v0" << std::endl;
				leftC = count;
				count++;
				mips_traverse_exp(exp->right, env);
				//result of right is now stored v0
				//fprintf(out, "move $t%d, $v0\n", count);
				out << "move $t" << count << ", $v0" << std::endl;
				rightC = count;
				count++;
			}
			
			print_exp_type(exp->kind);
			//fprintf(out, " $v0, $t%d, $t%d\n", leftC, rightC);
			out << " $v0, $t" << leftC << ", $t" << rightC << std::endl;
			count = count -2;
			
			exp->node_type = type_int_new();
			break;
		}
		case AST_EXP_LT:
		case AST_EXP_GT:{
			
			/*
			mips_traverse_exp(exp->left, env);
			out << "move $t" << count << ", $v0" << std::endl;
			leftC = count;
			count++;
			mips_traverse_exp(exp->right, env);
			*/
			break;
		}
		case AST_EXP_GT_EQ:
		case AST_EXP_LT_EQ:
		{
			
			break;
		}
		
		
		/*{
			  const Type* left = mips_traverse_exp(exp->left, env);
			  const Type* right = mips_traverse_exp(exp->right, env);

			  if (type_is_bool(left) && type_is_bool(right)) {
					exp->node_type =  type_bool_new();
			  }

			  break;
		}*/

		case AST_EXP_NOT: {/*
			  const Type* right = mips_traverse_exp(exp->right, env);

			  if (type_is_bool(right)) {
					exp->node_type = type_bool_new();
			  }
*/
			  break;
		}

		 /*
		{
			  const Type* left = mips_traverse_exp(exp->left, env);
			  const Type* right = mips_traverse_exp(exp->right, env);

			  if (type_is_int(left) && type_is_int(right)) {
					exp->node_type = type_bool_new();
			  }

			  break;
		}*/

		case AST_EXP_EQ:
		case AST_EXP_NOT_EQ: {/*
			const Type* left = mips_traverse_exp(exp->left, env);
			const Type* right = mips_traverse_exp(exp->right, env);

			if (type_equal(left, right) || (type_is_obj(left) && type_is_obj(right))) {
				exp->node_type = type_bool_new();
			}
*/
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
			//exp->node_type = type_copy_deep(env_lookup(env, exp->id));
			
			
			
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
			/*
			const Type* arr = type_expand(mips_traverse_exp(exp->left, env), env);
			const Type* idx = mips_traverse_exp(exp->right, env);

			if (type_is_array(arr) && type_is_int(idx)) {
				exp->node_type = type_copy_deep(type_array_get_eles(arr));
			}
			*/
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
			
			if (stmt->right->kind == AST_EXP_NUM)
			{
				out << "li $v0 " << stmt->right->num << std::endl;
			}
			else
			{
				const Type* right = mips_traverse_exp(stmt->right, env);
			}
			
			std::string id(symbol_to_str((stmt->left->id)));
			  
			int offset = local_variables[id];
			  
			//$vo contains result of right expression.
			out << "sw $v0, " << offset << "($fp)" << std::endl;
			  
			break;
		}

		case AST_STMT_IF: {
			  //const Type* cond = mips_traverse_exp(stmt->exp, env);
			int resultC;
			int resultcompareC;
			std::string elselabel = mips_label_gen();
			std::string endelselabel = mips_label_gen();
			
			//evaluate boolean expression
			mips_traverse_exp(stmt->exp, env);
			//store result in next available register
			//fprintf(out, "move $t%d, $v0\n", count);
			out << "move $t" << count << ", $v0" << std::endl;
			resultC = count;
			count++;
			//fprintf(out, "li $t%d, 1\n", count);
			out << "li $t" << count << ", 1" << std::endl;
			resultcompareC = count;
			count++;
			//check evaluation value
			//fprintf(out, "bnez $t%d, $t%d, %s\n", resultC, resultcompareC, elselabel.c_str());
			out << "benz $t" << resultC << ", $t" << resultcompareC << ", " << elselabel << std::endl;
			//jump to else if false
			g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);
			//jump to end else if true
			//fprintf(out, "li $t%d, 0 \n", resultcompareC);
			out << "li $t" << resultcompareC << ", 0" << std::endl;
			//fprintf(out, "beq $t%d, $t%d, %s\n", resultC, resultcompareC, endelselabel.c_str());
			out << "beq $t" << resultC << ", $t" << resultcompareC << ", " << endelselabel << std::endl;
			//print else label
			//fprintf(out, "%s:", elselabel.c_str());
			out << elselabel << ":" << std::endl;
			g_list_foreach(stmt->block2, (GFunc)mips_traverse_stmt, env);
			//print end else label
			//fprintf(out, "%s:", endelselabel.c_str());
			out << endelselabel << ":" << std::endl;
			
			break;
		}

		case AST_STMT_WHILE: {
			std::string while_label = mips_label_gen();
			int resultC;
			int resultcompareC;

			mips_traverse_exp(stmt->exp, env);
			out << "move $t" << count << ", $v0" << std::endl;
			resultC = count;
			count++;

			out << "li $t" << count << ", 0" << std::endl;
			resultcompareC = count;

			out << while_label << ":" << "beq $t" << resultC << ", $t" << resultcompareC << ", " << while_label << std::endl;
			g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);
		  	out << "j " << while_label << std::endl;
				
			break;
		}

		case AST_STMT_FOR: {
			const Type* lval = mips_traverse_exp(stmt->left, env);
			// const Type* from = mips_traverse_exp(stmt->exp, env);
			const Type* to = mips_traverse_exp(stmt->right, env);

			std::string for_label = mips_label_gen();
			std::string end_for_label = mips_label_gen();
			int resultC;
			int counter;

			mips_traverse_exp(stmt->exp, env);
			out << "move $t" << count << ", $v0" << std::endl;
			resultC = count;
			count++;

			//Load the counter
			out << "li $t" << count << ", 0" << std::endl;
			counter = count;
			count++;

			out << for_label << ": " << "beq $t" << resultC << ", $t" << counter << ", " << end_for_label << std::endl;
			g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);
			out << "addi $t" << counter << ", 1" << std::endl;
		  	out << "j " << for_label << std::endl;
		  	out << end_for_label << ":" << std::endl;

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
