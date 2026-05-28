#ifndef PPL_LEXER_H
#define PPL_LEXER_H

#include "diagnostic.h"
#include "token.h"

int lex_source(const char *source, Diagnostic *diag, TokenVec *out);
void token_vec_free(TokenVec *tokens);

#endif
