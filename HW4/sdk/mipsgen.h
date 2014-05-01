#ifndef LIGER_MIPSGEN_H_
#define LIGER_MIPSGEN_H_

#include "env.h"
#include "ast.h"
#include <glib.h>
#include <stdio.h>
void mips_generate_text(FILE* out, GList* decls, Env* env);
static void mips_traverse_decl(struct decl* d, Env* env);
static const Type* mips_traverse_exp(struct exp* exp, Env* env);
static const void mips_traverse_stmt(struct stmt* stmt, Env* env);
#endif