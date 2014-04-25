#ifndef LIGER_MIPSGEN_H_
#define LIGER_MIPSGEN_H_

#include "env.h"
#include <glib.h>

//void annotate_decls(GList* decls, Env* env);

//int decls_all_ok(GList* decls);
void mips_print_main(GList * ast_root);
#endif