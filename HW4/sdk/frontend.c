#include "typecheck.h"
#include "mipsgen.h"
#include "ast.h"
#include "env.h"
#include "frontend.h"m

#include "lexer.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <glib.h>

#include <iostream>
#include <fstream> 
#include <map>

int yyparse(void);

static void insert_decl(struct decl* d, Env* env);
static void insert_decls(GList* decls, Env* env);
static void insert_builtins(Env* env);
static int check_main_defined(void);

// A global variable for holding the root of the AST after parsing.
static GList* ast_root;
static Env* genv; 

int count;
int label_count;
int stack_count = 0;

//FILE* out;
std::ofstream out;
std::map<std::string, int> local_variables;
int reverse = 0;

void done_parsing(GList* parse_result) {
      ast_root = parse_result;
}

static void insert_decl(struct decl* d, Env* env) {
      assert(d);
      assert(d->type);
      assert(env);

      if (env_contains(env, d->id)) {
            d->node_type = type_conflict_new();
            return;
      }

      if (symbol_is_typename(d->id) && !type_is_array(d->type) && !type_is_struct(d->type)) {
            d->node_type = type_conflict_new();
            return;
      }

      if (symbol_is_fun(d->id)) {
            Env* fun_env = env_new();
            g_list_foreach(d->type->fun.params, (GFunc)insert_decl, fun_env);
            g_list_foreach(d->decls, (GFunc)insert_decl, fun_env);
            assert(d->type->fun.ret);
            env_insert(fun_env, symbol_fun_return(), d->type->fun.ret);
            env_insert_fun(env, d->id, d->type, fun_env, d->stmts);
            env_destroy(fun_env);
      } else {
            env_insert(env, d->id, d->type);
      }
}

static void insert_decls(GList* decls, Env* env) {
      g_list_foreach(ast_root, (GFunc)insert_decl, genv);
}

static void insert_builtins(Env* env) {
      // resize : ([A], int) -> [A]
      if (!env_contains(env, symbol_fun("resize"))) {
            GList* params = NULL;
            params = g_list_append(params, decl_param_new("arr", type_array_new(NULL)));
            params = g_list_append(params, decl_param_new("new_size", type_int_new()));
            Type* funtype = type_fun_new(params, type_array_new(NULL)); // Return type inferred from fun call.
            env_insert_fun(env, symbol_fun("resize"), funtype, NULL, NULL);
            type_destroy(funtype);
      }
      // sizeof : ([A]) -> int
      if (!env_contains(env, symbol_fun("sizeof"))) {
            GList* params = NULL;
            params = g_list_append(params, decl_param_new("arr", type_array_new(NULL)));
            Type* funtype = type_fun_new(params, type_int_new());
            env_insert_fun(env, symbol_fun("sizeof"), funtype, NULL, NULL);
            type_destroy(funtype);
      }
      // print : ([int]) -> void
      if (!env_contains(env, symbol_fun("print"))) {
            GList* params = NULL;
            params = g_list_append(params, decl_param_new("what", type_array_new(type_int_new())));
            Type* funtype = type_fun_new(params, type_void_new());
            env_insert_fun(env, symbol_fun("print"), funtype, NULL, NULL);
            type_destroy(funtype);
      }
      // getchar : () -> int
      if (!env_contains(env, symbol_fun("getchar"))) {
            GList* params = NULL;
            Type* funtype = type_fun_new(params, type_int_new());
            env_insert_fun(env, symbol_fun("getchar"), funtype, NULL, NULL);
            type_destroy(funtype);
      }
}

static int check_main_defined(void) {
      GList* params = NULL;
      params = g_list_append(params, decl_param_new("args", type_array_new(type_array_new(type_int_new()))));
      Type* maintype = type_fun_new(params, type_int_new());

      int main_defined = type_equal(maintype, env_lookup(genv, symbol_fun("main")));

      type_destroy(maintype);

      return main_defined;
}

static void generate_data(GList * ast_root, Env* genv)
{
	//mips_print_main(ast_root);
	//if(check_main_defined()==1)
	//fprintf(out,"you gonna get reminded");
	
	//fprintf(out, "newline:	.asciiz \"\\n\"");
	out << "newline:	.asciiz \"\\n\"" << std::endl;
	
}

static void generate_text(GList * ast_root, Env* genv)
{
	if(check_main_defined()==1)
		//fprintf(out,"\t\tmain:\n");
		out << "\t\tmain:\n";
		out << "move $fp $sp" << std::endl;
	
	
	///////////////////////////////////////////
	//do all asignment and declarations ( Local only )
	///////////////////////////////////////////
	mips_generate_text(ast_root, genv);
	
	
	//generate exit system call
	//fprintf(out, "\t\tli,$v0,10\n");
	//out << "\t\tli,$v0, 10\n";
	//fprintf(out, "\t\tsyscall\n");
	//out << "\t\tsyscall\n";
	
}

static void generate_mips(GList* ast_root, Env* genv)
{
	//fprintf(out, "\t\t.data\n");
	out << "\t\t.data\n";
	//generate data section
	generate_data(ast_root, genv);
		
	//fprintf(out, "\n\t\t.text\n");
	out << "\t\t.text\n";
	//generate text section
	generate_text(ast_root, genv);
	
}


int main(int argc, char** argv) {
      int ast_flag = 0, do_codegen = 1;
      yyin = NULL;
      count = 0;
      label_count = 0;

      if (argc < 2) {
            printf("Usage: %s [--ast] <liger_source_file>\n", argv[0]);
            return 1;
      }

      // Locate the "--ast" flag.
      for (int i = 1; i != argc; ++i) {
            if (!strcmp(argv[i], "--ast")) {
                  // Chop out "--ast" from the array of args.
                  if (i != (argc - 1)) {
                        argv[i] = argv[argc-1];
                  } 
                  --argc;

                  ast_flag = 1;
                  do_codegen = 0;
                  break;
            }
      }

      // For every non-"--ast"-flag arg, try to compile it.
      for (int i = 1; i != argc; ++i) {
            int prefix = strcspn(argv[i], ".");

            if (strcmp(argv[i] + prefix, ".lig")) {
                  fprintf(stderr, "Unrecognized file extension, skipping: %s\n", argv[i]);
                  continue;
            }

            yyrestart(fopen(argv[i], "r"));

            if (yyparse()) {
                  fprintf(stderr, "Error: failed to parse file: %s\n", argv[i]);

                  fclose(yyin);
                  continue;
            }

            genv = env_new();
            insert_decls(ast_root, genv);
            insert_builtins(genv);
			//call the function to set the OK or not OK type of every declaration
            annotate_decls(ast_root, genv);

            if (ast_flag) {
                  decls_print(ast_root);
                  puts("");
            }

            if (!check_main_defined()) {
                  printf("Function main() not defined or has wrong type!\n");
                  do_codegen = 0;
            } 

            if (!decls_all_ok(ast_root)) {
                  printf("Typechecking failed!\n");
                  do_codegen = 0;
            }

            if (do_codegen) {
				char* file_out = strdup(argv[i]);

				*(file_out + prefix) = 0; // Replace that "." in ".lig" with a null terminator.
				strcat(file_out, ".s"); // Add the new extension.
				//out = fopen(file_out, "w");
				out.open(file_out);
			
				// *** DO MIPS CODE GEN HERE. ***
				generate_mips(ast_root, genv);

				//fprintf(out, "move $a0, $v0\nli $v0, 1       # Select print_int syscall\nsyscall\n              la $a0, newline\n                li $v0, 4               # Select print_string syscall\n                syscall\nli $v0, 10\nsyscall");
				for( std::map<std::string, int>::iterator ii=local_variables.begin(); ii!=local_variables.end(); ++ii)
				{
					int offset = (*ii).second;
					/*
					//label
					//out << "li $a0, '" << (*ii).first << "'" << std::endl;//<< " = \"" << std::endl;
					out << "li $v0, 4" << std::endl;
					out << "syscall" << std::endl;
					*/
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
				
				///CLEAR STACK////
				//CLEAR THE STACK FROM ALL LOCAL VARIABLES! ASSUMPTION THAT WE ONLY HAVE MAIN - CHECK POINT 1
				out << "add $sp, $sp, " << local_variables.size() * 4 << std::endl;
				//decrease the stack counter variable since we are done with the variable
				stack_count = stack_count - local_variables.size()*4;
				/////////////////
				
				//exit command
				out << "li $v0, 10\n syscall";

				free(file_out);
				//fclose(out);
				out.close();
            }

            env_destroy(genv);
            decls_destroy(ast_root);
            fclose(yyin);
      }

      return 0;
}

