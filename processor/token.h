#ifndef PPL_TOKEN_H
#define PPL_TOKEN_H

typedef enum TokenKind {
    TOK_EOF,
    TOK_NEWLINE,
    TOK_EXPECT,
    TOK_IDENT,
    TOK_INT,
    TOK_LET,
    TOK_FN,
    TOK_IF,
    TOK_ELSE,
    TOK_RETURN,
    TOK_MATCH,
    TOK_SOME,
    TOK_NONE,
    TOK_FOR_EACH,
    TOK_IN,
    TOK_TRUE,
    TOK_FALSE,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COMMA,
    TOK_ASSIGN,
    TOK_FAT_ARROW,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PERCENT,
    TOK_EQ,
    TOK_NE,
    TOK_LT,
    TOK_LE,
    TOK_GT,
    TOK_GE,
    TOK_AND,
    TOK_OR,
    TOK_BANG
} TokenKind;

typedef struct Token {
    TokenKind kind;
    char *text;
    long int_value;
    int line;
    int column;
} Token;

typedef struct TokenVec {
    Token *items;
    int count;
    int capacity;
} TokenVec;

const char *token_kind_name(TokenKind kind);

#endif
