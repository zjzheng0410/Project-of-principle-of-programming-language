#include "parser.h"

#include <stdlib.h>
#include <string.h>

typedef struct Parser {
    TokenVec *tokens;
    int pos;
    Diagnostic *diag;
    int next_fn_id;
} Parser;

static Expr *parse_expr(Parser *p, int min_prec);
static Block *parse_block(Parser *p);

static Token *peek(Parser *p) {
    return &p->tokens->items[p->pos];
}

static Token *peek_n(Parser *p, int n) {
    int idx = p->pos + n;
    if (idx >= p->tokens->count) {
        return &p->tokens->items[p->tokens->count - 1];
    }
    return &p->tokens->items[idx];
}

static int at(Parser *p, TokenKind kind) {
    return peek(p)->kind == kind;
}

static Token *advance(Parser *p) {
    Token *token = peek(p);
    if (token->kind != TOK_EOF) {
        p->pos++;
    }
    return token;
}

static void skip_newlines(Parser *p) {
    while (at(p, TOK_NEWLINE)) {
        advance(p);
    }
}

static int consume(Parser *p, TokenKind kind, const char *message) {
    if (at(p, kind)) {
        advance(p);
        return 1;
    }
    diag_error(p->diag, peek(p)->line, peek(p)->column, "语法分析", "%s，当前 token 是 %s", message, token_kind_name(peek(p)->kind));
    return 0;
}

static char *copy_current_text(Parser *p) {
    return ast_strdup(peek(p)->text ? peek(p)->text : "");
}

/* 独立行 // => 是语句列表层语法，只能绑定当前 block/program 的最后一条语句，不能进入表达式解析后猜测处理。 */
static int attach_expect_to_previous(Parser *p, Block *block) {
    Token *token = peek(p);
    if (block->count == 0) {
        diag_error(p->diag, token->line, token->column, "语法分析", "期望输出注释缺少关联语句");
        advance(p);
        return 0;
    }

    Stmt *previous = block->items[block->count - 1];
    if (previous->expect) {
        diag_error(p->diag, token->line, token->column, "语法分析", "一条语句不能绑定多个期望输出注释");
        advance(p);
        return 0;
    }

    previous->expect = copy_current_text(p);
    advance(p);
    return 1;
}

static int looks_like_function_decl(Parser *p) {
    if (!at(p, TOK_IDENT) || peek_n(p, 1)->kind != TOK_LPAREN) {
        return 0;
    }
    int depth = 0;
    for (int i = p->pos + 1; i < p->tokens->count; i++) {
        TokenKind kind = p->tokens->items[i].kind;
        if (kind == TOK_LPAREN) {
            depth++;
        } else if (kind == TOK_RPAREN) {
            depth--;
            if (depth == 0) {
                return p->tokens->items[i + 1].kind == TOK_LBRACE;
            }
        } else if (kind == TOK_EOF || kind == TOK_NEWLINE) {
            return 0;
        }
    }
    return 0;
}

static int parse_param_list(Parser *p, StringList *params) {
    if (at(p, TOK_RPAREN)) {
        return 1;
    }
    for (;;) {
        if (!at(p, TOK_IDENT)) {
            diag_error(p->diag, peek(p)->line, peek(p)->column, "语法分析", "函数参数必须是标识符");
            return 0;
        }
        char *name = copy_current_text(p);
        advance(p);
        if (!name || !string_list_add(params, name)) {
            return 0;
        }
        if (!at(p, TOK_COMMA)) {
            break;
        }
        advance(p);
    }
    return 1;
}

static Stmt *parse_function_decl(Parser *p) {
    Token *name_tok = advance(p);
    char *name = ast_strdup(name_tok->text);
    consume(p, TOK_LPAREN, "函数声明缺少 '('");
    StringList params = {0};
    parse_param_list(p, &params);
    consume(p, TOK_RPAREN, "函数声明缺少 ')'");
    Block *body = parse_block(p);

    Expr *fn = expr_new(EXPR_FN, name_tok->line);
    fn->as.fn.params = params;
    fn->as.fn.body = body;
    fn->as.fn.fn_id = ++p->next_fn_id;

    Stmt *stmt = stmt_new(STMT_LET, name_tok->line);
    stmt->as.let_stmt.name = name;
    stmt->as.let_stmt.value = fn;
    return stmt;
}

static Stmt *parse_stmt(Parser *p) {
    skip_newlines(p);
    Token *start = peek(p);
    Stmt *stmt = NULL;
    if (looks_like_function_decl(p)) {
        stmt = parse_function_decl(p);
    } else if (at(p, TOK_LET)) {
        advance(p);
        if (!at(p, TOK_IDENT)) {
            diag_error(p->diag, peek(p)->line, peek(p)->column, "语法分析", "let 后缺少变量名");
            return NULL;
        }
        char *name = copy_current_text(p);
        advance(p);
        consume(p, TOK_ASSIGN, "let 语句缺少 '='");
        Expr *value = parse_expr(p, 0);
        stmt = stmt_new(STMT_LET, start->line);
        stmt->as.let_stmt.name = name;
        stmt->as.let_stmt.value = value;
    } else if (at(p, TOK_RETURN)) {
        advance(p);
        Expr *value = parse_expr(p, 0);
        stmt = stmt_new(STMT_RETURN, start->line);
        stmt->as.expr = value;
    } else if (at(p, TOK_FOR_EACH)) {
        advance(p);
        consume(p, TOK_LPAREN, "for_each 缺少 '('");
        if (!at(p, TOK_IDENT)) {
            diag_error(p->diag, peek(p)->line, peek(p)->column, "语法分析", "for_each 缺少循环变量");
            return NULL;
        }
        char *var = copy_current_text(p);
        advance(p);
        consume(p, TOK_IN, "for_each 缺少 in");
        Expr *array = parse_expr(p, 0);
        consume(p, TOK_RPAREN, "for_each 缺少 ')'");
        Block *body = parse_block(p);
        stmt = stmt_new(STMT_FOR_EACH, start->line);
        stmt->as.for_each.var = var;
        stmt->as.for_each.array = array;
        stmt->as.for_each.body = body;
    } else if (at(p, TOK_LBRACE)) {
        stmt = stmt_new(STMT_BLOCK, start->line);
        stmt->as.block = parse_block(p);
    } else if (at(p, TOK_IDENT) && peek_n(p, 1)->kind == TOK_ASSIGN) {
        char *name = copy_current_text(p);
        advance(p);
        advance(p);
        Expr *value = parse_expr(p, 0);
        stmt = stmt_new(STMT_ASSIGN, start->line);
        stmt->as.assign_stmt.name = name;
        stmt->as.assign_stmt.value = value;
    } else {
        Expr *expr = parse_expr(p, 0);
        stmt = stmt_new(STMT_EXPR, start->line);
        stmt->as.expr = expr;
    }
    if (at(p, TOK_EXPECT)) {
        stmt->expect = copy_current_text(p);
        advance(p);
    }
    while (at(p, TOK_NEWLINE)) {
        advance(p);
    }
    return stmt;
}

static Block *parse_block(Parser *p) {
    if (!consume(p, TOK_LBRACE, "块缺少 '{'")) {
        return NULL;
    }
    Block *block = block_new();
    skip_newlines(p);
    while (!at(p, TOK_RBRACE) && !at(p, TOK_EOF)) {
        if (at(p, TOK_EXPECT)) {
            if (!attach_expect_to_previous(p, block)) {
                skip_newlines(p);
                break;
            }
            skip_newlines(p);
            continue;
        }
        Stmt *stmt = parse_stmt(p);
        if (!stmt || !block_add(block, stmt)) {
            return block;
        }
        skip_newlines(p);
    }
    consume(p, TOK_RBRACE, "块缺少 '}'");
    return block;
}

static int precedence(TokenKind kind) {
    switch (kind) {
        case TOK_OR: return 1;
        case TOK_AND: return 2;
        case TOK_EQ:
        case TOK_NE: return 3;
        case TOK_LT:
        case TOK_LE:
        case TOK_GT:
        case TOK_GE: return 4;
        case TOK_PLUS:
        case TOK_MINUS: return 5;
        case TOK_STAR:
        case TOK_SLASH:
        case TOK_PERCENT: return 6;
        default: return 0;
    }
}

static const char *operator_text(TokenKind kind) {
    return token_kind_name(kind);
}

static Expr *parse_if_expr(Parser *p, int line) {
    Expr *expr = expr_new(EXPR_IF, line);
    if (at(p, TOK_LPAREN)) {
        advance(p);
        expr->as.if_expr.condition = parse_expr(p, 0);
        consume(p, TOK_RPAREN, "if 条件缺少 ')'");
    } else {
        expr->as.if_expr.condition = parse_expr(p, 0);
    }
    expr->as.if_expr.then_block = parse_block(p);
    skip_newlines(p);
    if (at(p, TOK_ELSE)) {
        advance(p);
        expr->as.if_expr.else_block = parse_block(p);
    }
    return expr;
}

static Expr *parse_fn_expr(Parser *p, int line) {
    Expr *expr = expr_new(EXPR_FN, line);
    expr->as.fn.fn_id = ++p->next_fn_id;
    if (at(p, TOK_LPAREN)) {
        advance(p);
        parse_param_list(p, &expr->as.fn.params);
        consume(p, TOK_RPAREN, "fn 参数列表缺少 ')'");
        expr->as.fn.body = parse_block(p);
        return expr;
    }
    for (;;) {
        if (!at(p, TOK_IDENT)) {
            diag_error(p->diag, peek(p)->line, peek(p)->column, "语法分析", "lambda 参数必须是标识符");
            return expr;
        }
        char *name = copy_current_text(p);
        advance(p);
        string_list_add(&expr->as.fn.params, name);
        if (!at(p, TOK_COMMA)) {
            break;
        }
        advance(p);
    }
    consume(p, TOK_FAT_ARROW, "lambda 缺少 '=>'");
    Block *body = block_new();
    Stmt *ret = stmt_new(STMT_EXPR, line);
    ret->as.expr = parse_expr(p, 0);
    block_add(body, ret);
    expr->as.fn.body = body;
    return expr;
}

static Expr *parse_match_expr(Parser *p, int line) {
    Expr *expr = expr_new(EXPR_MATCH, line);
    expr->as.match_expr.target = parse_expr(p, 0);
    consume(p, TOK_LBRACE, "match 缺少 '{'");
    skip_newlines(p);
    while (!at(p, TOK_RBRACE) && !at(p, TOK_EOF)) {
        MatchArm arm = {0};
        arm.line = peek(p)->line;
        if (at(p, TOK_SOME)) {
            arm.is_some = 1;
            advance(p);
            consume(p, TOK_LPAREN, "Some 模式缺少 '('");
            if (!at(p, TOK_IDENT)) {
                diag_error(p->diag, peek(p)->line, peek(p)->column, "语法分析", "Some 模式缺少绑定名");
                return expr;
            }
            arm.binding = copy_current_text(p);
            advance(p);
            consume(p, TOK_RPAREN, "Some 模式缺少 ')'");
            if (at(p, TOK_IF)) {
                advance(p);
                arm.guard = parse_expr(p, 0);
            }
        } else if (at(p, TOK_NONE)) {
            arm.is_some = 0;
            advance(p);
        } else {
            diag_error(p->diag, peek(p)->line, peek(p)->column, "语法分析", "match 分支必须是 Some(...) 或 None");
            return expr;
        }
        consume(p, TOK_FAT_ARROW, "match 分支缺少 '=>'");
        arm.value = parse_expr(p, 0);
        match_arm_list_add(&expr->as.match_expr.arms, arm);
        skip_newlines(p);
    }
    consume(p, TOK_RBRACE, "match 缺少 '}'");
    return expr;
}

static Expr *parse_primary(Parser *p) {
    Token *tok = advance(p);
    Expr *expr = NULL;
    switch (tok->kind) {
        case TOK_INT:
            expr = expr_new(EXPR_INT, tok->line);
            expr->as.int_value = tok->int_value;
            return expr;
        case TOK_TRUE:
        case TOK_FALSE:
            expr = expr_new(EXPR_BOOL, tok->line);
            expr->as.bool_value = tok->kind == TOK_TRUE;
            return expr;
        case TOK_IDENT:
            expr = expr_new(EXPR_VAR, tok->line);
            expr->as.var_name = ast_strdup(tok->text);
            return expr;
        case TOK_NONE:
            return expr_new(EXPR_NONE, tok->line);
        case TOK_SOME:
            expr = expr_new(EXPR_SOME, tok->line);
            consume(p, TOK_LPAREN, "Some 缺少 '('");
            expr->as.some_value = parse_expr(p, 0);
            consume(p, TOK_RPAREN, "Some 缺少 ')'");
            return expr;
        case TOK_LPAREN:
            expr = parse_expr(p, 0);
            consume(p, TOK_RPAREN, "表达式缺少 ')'");
            return expr;
        case TOK_LBRACKET:
            expr = expr_new(EXPR_ARRAY, tok->line);
            if (!at(p, TOK_RBRACKET)) {
                for (;;) {
                    Expr *item = parse_expr(p, 0);
                    expr_list_add(&expr->as.array_items, item);
                    if (!at(p, TOK_COMMA)) {
                        break;
                    }
                    advance(p);
                }
            }
            consume(p, TOK_RBRACKET, "数组字面量缺少 ']'");
            return expr;
        case TOK_IF:
            return parse_if_expr(p, tok->line);
        case TOK_FN:
            return parse_fn_expr(p, tok->line);
        case TOK_MATCH:
            return parse_match_expr(p, tok->line);
        default:
            diag_error(p->diag, tok->line, tok->column, "语法分析", "无法解析表达式，当前 token 是 %s", token_kind_name(tok->kind));
            return expr_new(EXPR_NONE, tok->line);
    }
}

static Expr *parse_unary(Parser *p) {
    if (at(p, TOK_BANG) || at(p, TOK_MINUS)) {
        Token *tok = advance(p);
        Expr *expr = expr_new(EXPR_UNARY, tok->line);
        expr->as.unary.op = ast_strdup(operator_text(tok->kind));
        expr->as.unary.expr = parse_unary(p);
        return expr;
    }
    Expr *expr = parse_primary(p);
    for (;;) {
        if (at(p, TOK_LPAREN)) {
            advance(p);
            Expr *call = expr_new(EXPR_CALL, expr->line);
            call->as.call.callee = expr;
            if (!at(p, TOK_RPAREN)) {
                for (;;) {
                    expr_list_add(&call->as.call.args, parse_expr(p, 0));
                    if (!at(p, TOK_COMMA)) {
                        break;
                    }
                    advance(p);
                }
            }
            consume(p, TOK_RPAREN, "函数调用缺少 ')'");
            expr = call;
        } else if (at(p, TOK_LBRACKET)) {
            advance(p);
            Expr *idx = expr_new(EXPR_INDEX, expr->line);
            idx->as.index.array = expr;
            idx->as.index.index = parse_expr(p, 0);
            consume(p, TOK_RBRACKET, "索引表达式缺少 ']'");
            expr = idx;
        } else {
            break;
        }
    }
    return expr;
}

static Expr *parse_expr(Parser *p, int min_prec) {
    Expr *left = parse_unary(p);
    while (precedence(peek(p)->kind) >= min_prec && precedence(peek(p)->kind) > 0) {
        Token *op = advance(p);
        int prec = precedence(op->kind);
        Expr *right = parse_expr(p, prec + 1);
        Expr *bin = expr_new(EXPR_BINARY, op->line);
        bin->as.binary.op = ast_strdup(operator_text(op->kind));
        bin->as.binary.left = left;
        bin->as.binary.right = right;
        left = bin;
    }
    return left;
}

Program *parse_program(TokenVec *tokens, Diagnostic *diag) {
    Parser p = {tokens, 0, diag, 0};
    Program *program = (Program *)calloc(1, sizeof(Program));
    if (!program) {
        return NULL;
    }
    program->body = block_new();
    skip_newlines(&p);
    while (!at(&p, TOK_EOF)) {
        if (at(&p, TOK_EXPECT)) {
            if (!attach_expect_to_previous(&p, program->body)) {
                break;
            }
            skip_newlines(&p);
            continue;
        }
        Stmt *stmt = parse_stmt(&p);
        if (!stmt || !block_add(program->body, stmt)) {
            break;
        }
        skip_newlines(&p);
    }
    program->function_count = p.next_fn_id;
    if (diag->error_count > 0) {
        program_free(program);
        return NULL;
    }
    return program;
}
