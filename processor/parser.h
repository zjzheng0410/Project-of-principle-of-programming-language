#ifndef PPL_PARSER_H
#define PPL_PARSER_H

#include "ast.h"
#include "diagnostic.h"
#include "token.h"

Program *parse_program(TokenVec *tokens, Diagnostic *diag);

#endif
