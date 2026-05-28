#ifndef PPL_SEMANTIC_H
#define PPL_SEMANTIC_H

#include "ast.h"
#include "diagnostic.h"

int semantic_check(Program *program, Diagnostic *diag);

#endif
