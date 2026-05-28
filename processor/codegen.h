#ifndef PPL_CODEGEN_H
#define PPL_CODEGEN_H

#include "ast.h"
#include "diagnostic.h"

int codegen_write_c(Program *program, const char *output_path, Diagnostic *diag);

#endif
