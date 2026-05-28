#include "ast.h"

#include <stdlib.h>
#include <string.h>

char *ast_strdup(const char *text) {
    size_t len = strlen(text);
    char *copy = (char *)malloc(len + 1);
    if (!copy) {
        return NULL;
    }
    memcpy(copy, text, len + 1);
    return copy;
}

Block *block_new(void) {
    Block *block = (Block *)calloc(1, sizeof(Block));
    return block;
}

static int grow_ptr_array(void ***items, int *capacity, int count) {
    if (count < *capacity) {
        return 1;
    }
    int next = *capacity ? *capacity * 2 : 8;
    void **grown = (void **)realloc(*items, sizeof(void *) * (size_t)next);
    if (!grown) {
        return 0;
    }
    *items = grown;
    *capacity = next;
    return 1;
}

int block_add(Block *block, Stmt *stmt) {
    if (!grow_ptr_array((void ***)&block->items, &block->capacity, block->count)) {
        return 0;
    }
    block->items[block->count++] = stmt;
    return 1;
}

Expr *expr_new(ExprKind kind, int line) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    if (!expr) {
        return NULL;
    }
    expr->kind = kind;
    expr->line = line;
    return expr;
}

Stmt *stmt_new(StmtKind kind, int line) {
    Stmt *stmt = (Stmt *)calloc(1, sizeof(Stmt));
    if (!stmt) {
        return NULL;
    }
    stmt->kind = kind;
    stmt->line = line;
    return stmt;
}

int expr_list_add(ExprList *list, Expr *expr) {
    if (!grow_ptr_array((void ***)&list->items, &list->capacity, list->count)) {
        return 0;
    }
    list->items[list->count++] = expr;
    return 1;
}

int string_list_add(StringList *list, char *text) {
    if (!grow_ptr_array((void ***)&list->items, &list->capacity, list->count)) {
        return 0;
    }
    list->items[list->count++] = text;
    return 1;
}

int match_arm_list_add(MatchArmList *list, MatchArm arm) {
    if (list->count == list->capacity) {
        int next = list->capacity ? list->capacity * 2 : 4;
        MatchArm *items = (MatchArm *)realloc(list->items, sizeof(MatchArm) * (size_t)next);
        if (!items) {
            return 0;
        }
        list->items = items;
        list->capacity = next;
    }
    list->items[list->count++] = arm;
    return 1;
}

static void expr_free(Expr *expr);

static void block_free(Block *block) {
    if (!block) {
        return;
    }
    for (int i = 0; i < block->count; i++) {
        Stmt *stmt = block->items[i];
        if (!stmt) {
            continue;
        }
        free(stmt->expect);
        switch (stmt->kind) {
            case STMT_LET:
                free(stmt->as.let_stmt.name);
                expr_free(stmt->as.let_stmt.value);
                break;
            case STMT_ASSIGN:
                free(stmt->as.assign_stmt.name);
                expr_free(stmt->as.assign_stmt.value);
                break;
            case STMT_EXPR:
            case STMT_RETURN:
                expr_free(stmt->as.expr);
                break;
            case STMT_FOR_EACH:
                free(stmt->as.for_each.var);
                expr_free(stmt->as.for_each.array);
                block_free(stmt->as.for_each.body);
                break;
            case STMT_BLOCK:
                block_free(stmt->as.block);
                break;
        }
        free(stmt);
    }
    free(block->items);
    free(block);
}

static void expr_list_free(ExprList *list) {
    for (int i = 0; i < list->count; i++) {
        expr_free(list->items[i]);
    }
    free(list->items);
}

static void string_list_free(StringList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->items[i]);
    }
    free(list->items);
}

static void expr_free(Expr *expr) {
    if (!expr) {
        return;
    }
    switch (expr->kind) {
        case EXPR_VAR:
            free(expr->as.var_name);
            break;
        case EXPR_SOME:
            expr_free(expr->as.some_value);
            break;
        case EXPR_ARRAY:
            expr_list_free(&expr->as.array_items);
            break;
        case EXPR_CALL:
            expr_free(expr->as.call.callee);
            expr_list_free(&expr->as.call.args);
            break;
        case EXPR_INDEX:
            expr_free(expr->as.index.array);
            expr_free(expr->as.index.index);
            break;
        case EXPR_BINARY:
            free(expr->as.binary.op);
            expr_free(expr->as.binary.left);
            expr_free(expr->as.binary.right);
            break;
        case EXPR_UNARY:
            free(expr->as.unary.op);
            expr_free(expr->as.unary.expr);
            break;
        case EXPR_FN:
            string_list_free(&expr->as.fn.params);
            block_free(expr->as.fn.body);
            break;
        case EXPR_IF:
            expr_free(expr->as.if_expr.condition);
            block_free(expr->as.if_expr.then_block);
            block_free(expr->as.if_expr.else_block);
            break;
        case EXPR_MATCH:
            expr_free(expr->as.match_expr.target);
            for (int i = 0; i < expr->as.match_expr.arms.count; i++) {
                free(expr->as.match_expr.arms.items[i].binding);
                expr_free(expr->as.match_expr.arms.items[i].guard);
                expr_free(expr->as.match_expr.arms.items[i].value);
            }
            free(expr->as.match_expr.arms.items);
            break;
        case EXPR_INT:
        case EXPR_BOOL:
        case EXPR_NONE:
            break;
    }
    free(expr);
}

void program_free(Program *program) {
    if (!program) {
        return;
    }
    block_free(program->body);
    free(program);
}
