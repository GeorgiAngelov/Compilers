#include "typecheck.h"
#include "type.h"
#include "ast.h"
#include "env.h"
#include <glib.h>
#include <assert.h>
#include <stdio.h>

void mips_find_print_main(Env* genv, FILE* out) {
	GList* params = NULL;
	params = g_list_append(params, decl_param_new("args", type_array_new(type_array_new(type_int_new()))));
	Type* maintype = type_fun_new(params, type_int_new());

	int main_defined = type_equal(maintype, env_lookup(genv, symbol_fun("main")));
	
	if(main_defined == 1){
		fprintf(out,"Mian declared");
	}
}

void mips_print_main(GList * ast_root, FILE* out){
	/*Env* genv = env_new();
	insert_decls(ast_root, genv);
	//decls_print(ast_root);
	g_list_foreach(genv, (GFunc)mips_find_print_main, NULL);*/
}