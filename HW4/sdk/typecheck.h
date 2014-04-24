#ifndef LIGER_TYPECHECKING_H_
#define LIGER_TYPECHECKING_H_

#include "env.h"
#include <glib.h>

void annotate_decls(GList* decls, Env* env);

int decls_all_ok(GList* decls);

#endif
