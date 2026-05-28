#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *slice_text(const char *src, int start, int end) {
    int len = end - start;
    char *s = (char *)malloc((size_t)len + 1);
    if (!s) {
        return NULL;
    }
    memcpy(s, src + start, (size_t)len);
    s[len] = '\0';
    return s;
}

static int push_token(TokenVec *vec, Token token) {
    if (vec->count == vec->capacity) {
        int next = vec->capacity ? vec->capacity * 2 : 128;
        Token *items = (Token *)realloc(vec->items, sizeof(Token) * (size_t)next);
        if (!items) {
            return 0;
        }
        vec->items = items;
        vec->capacity = next;
    }
    vec->items[vec->count++] = token;
    return 1;
}

static TokenKind keyword_kind(const char *text) {
    if (strcmp(text, "let") == 0 || strcmp(text, "Let") == 0) return TOK_LET;
    if (strcmp(text, "fn") == 0) return TOK_FN;
    if (strcmp(text, "if") == 0) return TOK_IF;
    if (strcmp(text, "else") == 0) return TOK_ELSE;
    if (strcmp(text, "return") == 0) return TOK_RETURN;
    if (strcmp(text, "match") == 0) return TOK_MATCH;
    if (strcmp(text, "Some") == 0) return TOK_SOME;
    if (strcmp(text, "None") == 0) return TOK_NONE;
    if (strcmp(text, "for_each") == 0) return TOK_FOR_EACH;
    if (strcmp(text, "in") == 0) return TOK_IN;
    if (strcmp(text, "true") == 0) return TOK_TRUE;
    if (strcmp(text, "false") == 0) return TOK_FALSE;
    return TOK_IDENT;
}

int lex_source(const char *source, Diagnostic *diag, TokenVec *out) {
    int i = 0;
    int line = 1;
    int col = 1;
    out->items = NULL;
    out->count = 0;
    out->capacity = 0;

    while (source[i]) {
        char ch = source[i];
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            i++;
            col++;
            continue;
        }
        if (ch == '\n') {
            Token t = {TOK_NEWLINE, NULL, 0, line, col};
            if (!push_token(out, t)) return 0;
            i++;
            line++;
            col = 1;
            continue;
        }
        if (ch == '/' && source[i + 1] == '/') {
            int start_line = line;
            int start_col = col;
            i += 2;
            col += 2;
            while (source[i] == ' ' || source[i] == '\t') {
                i++;
                col++;
            }
            if (source[i] == '=' && source[i + 1] == '>') {
                i += 2;
                col += 2;
                while (source[i] == ' ' || source[i] == '\t') {
                    i++;
                    col++;
                }
                int start = i;
                while (source[i] && source[i] != '\n') {
                    i++;
                    col++;
                }
                int end = i;
                while (end > start && (source[end - 1] == ' ' || source[end - 1] == '\t')) {
                    end--;
                }
                Token t = {TOK_EXPECT, slice_text(source, start, end), 0, start_line, start_col};
                if (!t.text || !push_token(out, t)) return 0;
            } else {
                while (source[i] && source[i] != '\n') {
                    i++;
                    col++;
                }
            }
            continue;
        }
        if (isalpha((unsigned char)ch) || ch == '_') {
            int start = i;
            int start_col = col;
            while (isalnum((unsigned char)source[i]) || source[i] == '_') {
                i++;
                col++;
            }
            char *text = slice_text(source, start, i);
            if (!text) return 0;
            TokenKind kind = keyword_kind(text);
            Token t = {kind, text, 0, line, start_col};
            if (!push_token(out, t)) return 0;
            continue;
        }
        if (isdigit((unsigned char)ch)) {
            int start = i;
            int start_col = col;
            long value = 0;
            while (isdigit((unsigned char)source[i])) {
                value = value * 10 + (source[i] - '0');
                i++;
                col++;
            }
            Token t = {TOK_INT, slice_text(source, start, i), value, line, start_col};
            if (!t.text || !push_token(out, t)) return 0;
            continue;
        }

        Token t = {TOK_EOF, NULL, 0, line, col};
        if (ch == '=' && source[i + 1] == '>') {
            t.kind = TOK_FAT_ARROW;
            i += 2;
            col += 2;
        } else if (ch == '=' && source[i + 1] == '=') {
            t.kind = TOK_EQ;
            i += 2;
            col += 2;
        } else if (ch == '!' && source[i + 1] == '=') {
            t.kind = TOK_NE;
            i += 2;
            col += 2;
        } else if (ch == '<' && source[i + 1] == '=') {
            t.kind = TOK_LE;
            i += 2;
            col += 2;
        } else if (ch == '>' && source[i + 1] == '=') {
            t.kind = TOK_GE;
            i += 2;
            col += 2;
        } else if (ch == '&' && source[i + 1] == '&') {
            t.kind = TOK_AND;
            i += 2;
            col += 2;
        } else if (ch == '|' && source[i + 1] == '|') {
            t.kind = TOK_OR;
            i += 2;
            col += 2;
        } else {
            switch (ch) {
                case '(': t.kind = TOK_LPAREN; break;
                case ')': t.kind = TOK_RPAREN; break;
                case '{': t.kind = TOK_LBRACE; break;
                case '}': t.kind = TOK_RBRACE; break;
                case '[': t.kind = TOK_LBRACKET; break;
                case ']': t.kind = TOK_RBRACKET; break;
                case ',': t.kind = TOK_COMMA; break;
                case '=': t.kind = TOK_ASSIGN; break;
                case '+': t.kind = TOK_PLUS; break;
                case '-': t.kind = TOK_MINUS; break;
                case '*': t.kind = TOK_STAR; break;
                case '/': t.kind = TOK_SLASH; break;
                case '%': t.kind = TOK_PERCENT; break;
                case '<': t.kind = TOK_LT; break;
                case '>': t.kind = TOK_GT; break;
                case '!': t.kind = TOK_BANG; break;
                default:
                    diag_error(diag, line, col, "词法分析", "无法识别字符 '%c'", ch);
                    i++;
                    col++;
                    continue;
            }
            i++;
            col++;
        }
        if (!push_token(out, t)) return 0;
    }
    Token eof = {TOK_EOF, NULL, 0, line, col};
    return push_token(out, eof);
}

void token_vec_free(TokenVec *tokens) {
    for (int i = 0; i < tokens->count; i++) {
        free(tokens->items[i].text);
    }
    free(tokens->items);
    tokens->items = NULL;
    tokens->count = 0;
    tokens->capacity = 0;
}

const char *token_kind_name(TokenKind kind) {
    switch (kind) {
        case TOK_EOF: return "EOF";
        case TOK_NEWLINE: return "newline";
        case TOK_EXPECT: return "expect-comment";
        case TOK_IDENT: return "identifier";
        case TOK_INT: return "integer";
        case TOK_LET: return "let";
        case TOK_FN: return "fn";
        case TOK_IF: return "if";
        case TOK_ELSE: return "else";
        case TOK_RETURN: return "return";
        case TOK_MATCH: return "match";
        case TOK_SOME: return "Some";
        case TOK_NONE: return "None";
        case TOK_FOR_EACH: return "for_each";
        case TOK_IN: return "in";
        case TOK_TRUE: return "true";
        case TOK_FALSE: return "false";
        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_LBRACKET: return "[";
        case TOK_RBRACKET: return "]";
        case TOK_COMMA: return ",";
        case TOK_ASSIGN: return "=";
        case TOK_FAT_ARROW: return "=>";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_STAR: return "*";
        case TOK_SLASH: return "/";
        case TOK_PERCENT: return "%";
        case TOK_EQ: return "==";
        case TOK_NE: return "!=";
        case TOK_LT: return "<";
        case TOK_LE: return "<=";
        case TOK_GT: return ">";
        case TOK_GE: return ">=";
        case TOK_AND: return "&&";
        case TOK_OR: return "||";
        case TOK_BANG: return "!";
    }
    return "?";
}
