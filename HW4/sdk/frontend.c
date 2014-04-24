#include "typecheck.h"
#include "ast.h"
#include "env.h"
#include "frontend.h"

#include "lexer.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <glib.h>

int yyparse(void);

static void insert_decl(struct decl* d, Env* env);
static void insert_decls(GList* decls, Env* env);
static void insert_builtins(Env* env);
static int check_main_defined(void);

// A global variable for holding the root of the AST after parsing.
static GList* ast_root;
static Env* genv;

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

int main(int argc, char** argv) {
      int ast_flag = 0, do_codegen = 1;
      yyin = NULL;

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

                  FILE* out = fopen(file_out, "w");

                  // *** DO MIPS CODE GEN HERE. ***
                  // generate_mips(out, ast_root, genv);

                  free(file_out);
                  fclose(out);
            }

            env_destroy(genv);
            decls_destroy(ast_root);
            fclose(yyin);
      }

      return 0;
}

