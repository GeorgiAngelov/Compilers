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
#include <vector>


enum {
     SYMBOL_INVALID = 256,
     SYMBOL_FIELD,
     SYMBOL_FUN,
     SYMBOL_TYPENAME,
     SYMBOL_VAR,
     SYMBOL_PSEUDO,
};

enum{
	fun_decl,
	main,
	fun_exec,
};

extern int count;

#define INVALID_VALUE -500

extern int label_count;
//extern FILE *out;
extern std::ofstream out;
extern int reverse;


std::string mode = "none";

int in_execution = 0;

int inception = 0;

struct function_data{
std::string fun_name;
int arguments;
int preserved_registers;
int local_data;
};


extern std::map<std::string, int> local_variables;
std::map<std::string,function_data> function_map;

//extern std::map<std::string, int> local_variables;
//this is the map that holds all variables
//0 will be all global variables, 1 will be all main variables,
//and the indices after that will be every function call
extern std::vector< std::map<std::string, int> > variables;
Symbol current_fun;
int return_complete = 0;
extern int stack_count;


std::string MAIN_LABEL = "main";

	
//function to generate a the next label ( done so we do not interfere with previously created labels )
std::string mips_label_gen() {
	std::string label = "_lbl";

	std::string s;
	//input the label_count as a string
	std::stringstream out;
	out << label_count;
	s = out.str();
	//append the count number to the "label" string
	label.append(s);  
	label_count++;

	return label;
}

int get_variable_offset(std::string id){
	
	std::map<std::string,int>::iterator it = variables.back().find(id);
	int offset;
	//the variable is declared inside the function so access it
	if(symbol_equal(current_fun, symbol_fun("main"))){
		offset = variables[1][id];
	}
	else if(it != variables.back().end())
	{
		offset = it->second;
	}
	//the variable is a global variable(not declared in function)
	else{
		offset = variables[0][id];
	}
	
	return offset;
}

//prints how many current variable environments we have
void print_variables_environments(){
	//std::cout << "Current vector::variable length is " << variables.size() << std::endl;
}

//function return statements
void create_return(Symbol current_fun, Env* env)
{
	if (symbol_equal(current_fun, symbol_fun("main")))
			{
				/*
				for( std::map<std::string, int>::iterator ii=variables[0].begin(); ii!=variables[0].end(); ++ii)
				{
					int offset = (*ii).second;
					
					//label
					//out << "li $a0, '" << (*ii).first << "'" << std::endl;//<< " = \"" << std::endl;
					out << "li $v0, 4" << std::endl;
					out << "syscall" << std::endl;
					
					//value
					std::cout << " symbol: " << (*ii).first << " | " << offset << "($fp)"<< std::endl;
					out << "lw $a0, " << offset << "($fp)" << std::endl;
					//out << "li " << offset << "($fp), 1" << std::endl;
					out << "li $v0, 1" << std::endl;
					out << "syscall" << std::endl;
					
					//newline 
					out << "la $a0, newline" << std::endl;
					out << "li $v0, 4" << std::endl;
					out << "syscall" << std::endl;
				}
				*/
				
				///CLEAR STACK////
				//CLEAR THE STACK FROM ALL LOCAL VARIABLES! ASSUMPTION THAT WE ONLY HAVE MAIN - CHECK POINT 1
				out << "add $sp, $sp, " << (variables[0].size() * 4 + variables[1].size() * 4) << std::endl;
				for (int i = 0; i < variables.size(); i++)
				{
					for (std::map<std::string,int>::iterator it=variables[i].begin(); it!=variables[i].end(); ++it)
					{
    						std::cout << "variables[" << i << "][" << "] = " << it->first << std::endl;
					}
				}
				//std::cout << "local to main: " << variables[1].size() << std::endl;
				//std::cout << "global: " << variables[0].size() << std::endl;
				//decrease the stack counter variable since we are done with the variable
				stack_count = stack_count - variables[0].size()*4;
				stack_count = stack_count - variables[1].size()*4;
				/////////////////
				
				//exit command
				out << "li $v0, 10\n syscall" << std::endl;
			}
			else
			{
				//lookup function:
				Env * en = env_lookup_fun_env(env, current_fun);
				
				//free local variables
				out << "add $sp, $sp, " << g_hash_table_size(en->vars) * 4 << std::endl;
				
				//free function parameters (or are these included in local variable count? I think they are)
				
				out << "jr $ra #" << symbol_to_str(current_fun) << std::endl;	
			}
}

void insert_var(std::string id, int stack_count){
	//if we are dealing with global vars
	if(current_fun.kind == INVALID_VALUE){
		std::cout << "Inserting value into global" << std::endl;
		variables[0][id] = stack_count;
	}
	//if the current function is main
	else if (symbol_equal(current_fun, symbol_fun("main")))
	{
		std::cout << "Inserting value " << id << " into MAIN" << std::endl;
		variables[1][id] = stack_count;
	}
	//if we are in any other function, get the	 back!
	else
	{
		std::cout << "Not in main so I am inserting at last function I am in and var id is " << id << std::endl;
		std::cout << " variable count is " << variables.size() << std::endl;
		variables.back()[id] = stack_count;
	}
}

//local declarations
void mips_generate_text(GList * decls, Env* env){
	//default value of current_fun to know we are in global in the begining.
	current_fun.kind = INVALID_VALUE;
	
	//std::cout << "before inserting global into variables" << std::endl;
	print_variables_environments();
	
	//INSERT THE MAP FOR THE GLOBAL DECLARATIONS
	//THIS IS INDEX 0 IN variables 
	std::map<std::string, int> tmp;
	//push the new map into the the vector
	variables.push_back(tmp);
	
	//loop through each declaration in the tree
	g_list_foreach(decls, (GFunc)mips_traverse_decl, env);
}

static void mips_traverse_decl(struct decl* d, Env* env) {
	Type* calculated = NULL;
	printf(symbol_to_str(d->id));
	printf("\n");

	//if the declaration has any expressions
	if (d->exp) {
		//const Type* t = 
		mips_traverse_exp(d->exp, env);
		//This case is "var i:int  = 5"
		if(symbol_is_var(d->id)){
			std::cout << "symbol_is_var is hit and id is " << symbol_to_str(d->id) << std::endl;
			out << "sub $sp, $sp, 4" << std::endl;
			//set up var access in map -- looks up the id
			std::string id(symbol_to_str((d->id)));
			//this increments the stack count, the current offset
			stack_count += 4;
			//std::cout << "before storing variable after expression and environment is: " << std::endl;
			print_variables_environments();
			//thus is stored on a map based on the id
			//std::cout << symbol_to_str(current_fun) << std::endl;
			//insert into var
			insert_var(id, stack_count);
			
			//store the evaluation of the expression and store 
			out << "sw $v0, " << stack_count << "($fp)" << std::endl;
		}else{
			std::cout << " Expression is not an assignmnet but just 'x = 5' " << symbol_to_str(d->id) << std::endl;
		}
	}
	//if the declaration has any declarations or statements(hence it is a function)
	else if (d->decls || d->stmts) {

		out << "jr $ra" << std::endl;
		printf("jr $ra\n");
		out << symbol_to_str(d->id) << ":" << std::endl;
		std::string function_name = symbol_to_str(d->id);
		if(function_name == "main")
			in_execution = 1;
		//setting mode
		mode = symbol_to_str(d->id);

		function_data func;

		function_map[function_name] = func;

		//std::cout << "it is either a d-decls or d->stmts" << symbol_to_str((d->id)) << std::endl;
		print_variables_environments();

		Env* fun_env = env_lookup_fun_env(env, d->id);
		std::cout << "symbol_to_str is " << symbol_to_str(d->id) << std::endl;
		//make space in the vector variables for main if we are declaring main
		if(MAIN_LABEL.compare(symbol_to_str(d->id)) == 0){
			std::cout << " inserting 'main' " << symbol_to_str(d->id) << std::endl;
			//INSERT THE MAP FOR "MAIN's" DECLARATIONS
			//THIS IS INDEX 1 IN variables 
			std::map<std::string, int> tmp;
			//push the new map into the the vector
			  std::vector<std::map<std::string, int> >::iterator it = variables.begin();
			  it++;
			//std::cout << "main insertion variables.count " << variables.size() << std::endl;
			variables.insert(it, tmp);
			//variables.push_back(tmp);
		}
		else
		{
			std::cout << " inserting a new function and it's id is " << symbol_to_str(d->id) << std::endl;
			std::map<std::string, int> tmp;
			variables.push_back(tmp);
		}
		out << "\t\t" << symbol_to_str(d->id) << ":" << std::endl;
		current_fun = d->id;
		
		Env* merged_env = env_union(env, fun_env);
		
		return_complete = 0;
		
		g_list_foreach(d->decls, (GFunc)mips_traverse_decl, merged_env);
		g_list_foreach(d->stmts, (GFunc)mips_traverse_stmt, merged_env);

		
		if (return_complete == 0)
		{
			create_return(d->id, env);
		}
		

		env_destroy(merged_env);
		
		// pop variables off the stack
		
		//std::cout << "after decls or stmts" << std::endl;
		print_variables_environments();
	}
	//this gets hit if we have "var x:int;"
	else
	{
		std::cout << "it is neither and symbol is: " << symbol_to_str((d->id)) << std::endl;
		print_variables_environments();
		//How to reserve space on the stack for local
		//Also below is the reserve space
		out << "sub $sp, $sp, 4" << std::endl;
		//set up var access in m	ap -- looks up the id
		std::string id(symbol_to_str((d->id)));
		//this increments the stack count, the current offset
		stack_count += 4;
		//insert a var
		insert_var(id, stack_count);
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
		case AST_EXP_LT:
		{
			out << "slt";
			break;
		}
		case AST_EXP_GT:
		{
			out << "sgt";
			break;
		}
		case AST_EXP_GT_EQ:
		{
			out << "sge";
			break;
		}
		case AST_EXP_LT_EQ:
		{
			out << "sle";
			break;
		}
		case AST_EXP_EQ:
		{
			out << "seq";
			break;
		}
		case AST_EXP_NOT_EQ:
		{
			out << "sne";
			break;
		}
	}
	
}

const Type* mips_traverse_exp(struct exp* exp, Env* env) {
	exp->node_type = NULL;

	switch (exp->kind) {
		case AST_EXP_PLUS:
		case AST_EXP_MINUS:
		case AST_EXP_DIV:
		case AST_EXP_MOD:
		case AST_EXP_MUL: 
		case AST_EXP_OR:
		case AST_EXP_AND: 
		case AST_EXP_LT:
		case AST_EXP_GT:
		case AST_EXP_GT_EQ:
		case AST_EXP_LT_EQ:
		case AST_EXP_EQ:
		case AST_EXP_NOT_EQ: {
			
			//Smarter version
			
			if (exp->left->kind == AST_EXP_NUM || 
				exp->left->kind == AST_EXP_TRUE ||
				exp->left->kind == AST_EXP_FALSE)
			{
				//do right first
				mips_traverse_exp(exp->right, env);
				
				out << "move $v1, $v0" << std::endl;
				
				mips_traverse_exp(exp->left, env);
				
				//print the type of expression(add, sub, mul, etc...)
				print_exp_type(exp->kind);
				
				out << " $v0, $v0, $v1" << std::endl;
			}
			else if (exp->right->kind == AST_EXP_NUM || 
				exp->right->kind == AST_EXP_TRUE ||
				exp->right->kind == AST_EXP_FALSE)
			{
				//do left first
				mips_traverse_exp(exp->left, env);
				
				out << "move $v1, $v0" << std::endl;
				
				mips_traverse_exp(exp->right, env);
				
				//print the type of expression
				print_exp_type(exp->kind);
				
				out << " $v0, $v1, $v0" << std::endl;
			}
			else
			{
			//Simple Version
			
			//create a whole for the value of this expression
			out << "sub $sp, $sp, 4" << std::endl;
			//increment our stack_count variable for offsets
			stack_count += 4;
			//traverset the left side of the expression
			mips_traverse_exp(exp->left, env);
			
			//store the result from the left side onto the stack
			out << "sw $v0, 0($sp)"<< std::endl;
			
			//traverse the right side of the expression
			mips_traverse_exp(exp->right, env);
			
			//store the results from the right expression into $v1
			out << "lw $v1, 0($sp)" << std::endl;
			
			//decrement the stack (that way we are back to the original stack length
			//that was right before we entered this case
			out << "add $sp, $sp, 4" << std::endl;
			
			//decrease the stack counter variable since we are done with the variable
			stack_count = stack_count - 4;
			
			//print the type of epression(add, sub, mul, etc...)
			print_exp_type(exp->kind);
			
			out << " $v0, $v1, $v0" << std::endl;
			}
			
			
			
			exp->node_type = type_int_new();
			break;
		}
		case AST_EXP_NOT: {
			//traverse the right side of the expression
			mips_traverse_exp(exp->right, env);
			
			out << "not $v0, $v0" << std::endl;
			
			if (exp->right->kind == TYPE_BOOL) {
				exp->node_type = type_bool_new();
			}
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

		 {/*
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
			out << "li $v0, " << exp->num << std::endl;
			break;
		}

		case AST_EXP_TRUE: {
			exp->node_type = type_bool_new();
			out << "li $v0, 1" << std::endl;
			break;
		}

		case AST_EXP_FALSE: {
			exp->node_type = type_bool_new();
			out << "li $v0, 0" << std::endl;
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
			std::string id(symbol_to_str((exp->id)));
			  
			int offset = get_variable_offset(id);
			  
			//$vo contains result of right expression.
			out << "lw $v0, " << offset << "($fp)" << std::endl;
			
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
			std::string compare = symbol_to_str(exp->id);
			if (compare != "print"){
			//need to call function to print the "print" function in MIPS
			//if not print, ordinary function prologue
			//out << "addiu $sp,$sp,128" << "#push stack frame" << std::endl;
			//out << "jal " << compare << std::endl;
			mode = compare;
			std::string parameter;
			GList *glist_param;
			glist_param = g_list_first(exp->params);
			
							out << "jal " << compare << std::endl;
				//mode = compare;
				//create a new map for the new function call
				std::map<std::string, int> tmp;
				//push the new map into the the vector
				variables.push_back(tmp);
			
			//exps_print(glist_param);
			
			inception++;
			}
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
	//out << "# mode is " << mode << std::endl;

	switch (stmt->kind) {
		case AST_STMT_EXP: {
			  if (mips_traverse_exp(stmt->exp, env)) {
					stmt->node_type = type_ok_new();
			  }
			  break;
		}
		case AST_STMT_ASSIGN: {		
			//checks if the number being assigned is just a num, if exp then looks up the value
			if (stmt->right->kind == AST_EXP_NUM)
			{
				out << "li $v0 " << stmt->right->num << std::endl;
			}
			else
			{
				//const Type* right = 
				mips_traverse_exp(stmt->right, env);
			}

			//check if we are in a function declaration
			out << "# mode is " << mode << std::endl;
		
			//After end, value will be stored $v0
			
			//Then we do ID lookup
			std::string id(symbol_to_str((stmt->left->id)));
			
			//Find the particular symbol's offset  
			int offset = get_variable_offset(id);
			
			//This is where the storage of $v0 happens  
			//$vo contains result of right expression.
			out << "sw $v0, " << offset << "($fp)" << std::endl;
			 
			break;
		}

		case AST_STMT_IF: {
			  //const Type* cond = mips_traverse_exp(stmt->exp, env);
			//int resultC;
			int resultcompareC;
			std::string elselabel = mips_label_gen();
			std::string endelselabel = mips_label_gen();
			
			//evaluate boolean expression
			mips_traverse_exp(stmt->exp, env);
			//store result in next available register
			//fprintf(out, "move $t%d, $v0\n", count);
			//out << "move $t" << count << ", $v0" << std::endl;
			//resultC = count;
			//count++;
			//fprintf(out, "li $t%d, 1\n", count);
			//out << "li $t" << count << ", 1" << std::endl;
			resultcompareC = count;
			count++;
			//check evaluation value
			//fprintf(out, "bnez $t%d, $t%d, %s\n", resultC, resultcompareC, elselabel.c_str());
			out << "beq $v0, 0, " << elselabel << std::endl;
			//jump to else if false
			g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);
			//jump to end else if true
			//fprintf(out, "li $t%d, 0 \n", resultcompareC);
			//out << "li $t" << resultcompareC << ", 0" << std::endl;
			//fprintf(out, "beq $t%d, $t%d, %s\n", resultC, resultcompareC, endelselabel.c_str());
			//out << "beq $t" << resultC << ", $t" << resultcompareC << ", " << endelselabel << std::endl;
			out << "j " << endelselabel << std::endl;
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
			//out << "sub $sp, $sp, 4" << std::endl;
			//stack_count += 4;

			std::string while_label = mips_label_gen();
			std::string end_while_label = mips_label_gen();
			//int resultC;
			//int resultcompareC;

			//out << while_label << ":" << "beq 0($sp)" << ", 0, " << while_label << std::endl;
			out << while_label << ":" << std::endl;
			mips_traverse_exp(stmt->exp, env);
			//out << "sw $v0, 0($sp)" << std::endl;
			out << "li $v1, 0" << std::endl;
			out << "beq $v0" << ", $v1, " << end_while_label << std::endl;
			g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);
		  	out << "j " << while_label << std::endl;
		  	out << end_while_label << ":" << std::endl;
			
			//stack_count -= 4;
			//out << "add $sp, $sp, 4" << std::endl;
			break;
		}

		case AST_STMT_FOR: {
			//const Type* lval = mips_traverse_exp(stmt->left, env);
			// const Type* from = mips_traverse_exp(stmt->exp, env);
			//const Type* to = mips_traverse_exp(stmt->right, env);

			//left is the variable I look up (i)
			//expr is what I set i equal to initially
			//right is the expression I check against.
			
			//find value of expr
			mips_traverse_exp(stmt->exp, env);

			//Then we do ID lookup
			std::string id(symbol_to_str((stmt->left->id)));
			
			//Find the particular symbol's offset  
			int offset = get_variable_offset(id);// local_variables[id];
			
			//assign initial value to counter
			out << "sw $v0, " << offset << "($fp)" << std::endl;

			std::string for_label = mips_label_gen();
			std::string end_for_label = mips_label_gen();
			//int resultC;
			//int counter;
			
			//print for label
			out << for_label << ":" << std::endl;
			
			//do comparison
			mips_traverse_exp(stmt->right, env);
			out << "lw $v1, " << offset << "($fp)" << std::endl;
			out << "bge $v1, $v0, " << end_for_label << std::endl;
			
			//execute block
			g_list_foreach(stmt->block1, (GFunc)mips_traverse_stmt, env);
			
			//increment counter
			out << "lw $v0, " << offset << "($fp)" << std::endl;
			out << "addi $v0, $v0, 1" << std::endl;
			out << "sw $v0, " << offset << "($fp)" << std::endl;
			
			
			//loop
			out << "j " << for_label << std::endl;
			
			//print end for label
			out << end_for_label << ":" << std::endl;

			break;
		}

		case AST_STMT_RETURN: {
			  //Type* expected = 
			  //env_lookup(env, symbol_fun_return());
			
			//Env * en = env_lookup_fun_env(env, symbol_fun_return());
			
			
			  Type* actual = NULL;
			  if (stmt->exp) {
					actual = type_copy_deep(mips_traverse_exp(stmt->exp, env));
			  } else {
					actual = type_void_new();
			  }
			  //function epilogue
			  //out << "addiu $sp,$sp,128" << std::endl;
			  if(in_execution == 0)
 			  	//out << "jr $31" << std::endl;
			  if(in_execution == 1){
			  	printf("end main\n");
			  }
			  
			  create_return(current_fun, env);
			return_complete = 1;
			  type_destroy(actual);
			  break;
		}
	}
}
