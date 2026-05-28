#include "semantic.h"

#include <stdlib.h>
#include <string.h>

typedef struct Name {
    char *text;
    struct Name *next;
} Name;

typedef struct Scope {
    Name *names;
    struct Scope *parent;
} Scope;

static const char *builtins[] = {
    "is_some", "is_none", "map_opt", "unwrap_or", "and_then",
    "len", "map", "filter", "reduce", "any", "all", NULL
};

static int is_builtin(const char *name) {
    for (int i = 0; builtins[i]; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static void scope_define(Scope *scope, char *name) {
    Name *item = (Name *)calloc(1, sizeof(Name));
    item->text = name;
    item->next = scope->names;
    scope->names = item;
}

static int scope_find(Scope *scope, const char *name) {
    for (Scope *s = scope; s; s = s->parent) {
        for (Name *n = s->names; n; n = n->next) {
            if (strcmp(n->text, name) == 0) {
                return 1;
            }
        }
    }
    return is_builtin(name);
}

static void scope_free_names(Scope *scope) {
    Name *n = scope->names;
    while (n) {
        Name *next = n->next;
        free(n);
        n = next;
    }
}

static void check_expr(Expr *expr, Scope *scope, Diagnostic *diag);
static void check_block(Block *block, Scope *parent, Diagnostic *diag, int create_scope);

static void check_stmt(Stmt *stmt, Scope *scope, Diagnostic *diag) {
    switch (stmt->kind) {
        case STMT_LET:
            /* 先登记名字，允许递归函数在函数体内引用自身。 */
            scope_define(scope, stmt->as.let_stmt.name);
            check_expr(stmt->as.let_stmt.value, scope, diag);
            break;
        case STMT_ASSIGN:
            if (!scope_find(scope, stmt->as.assign_stmt.name)) {
                diag_error(diag, stmt->line, 1, "语义检查", "未定义变量 '%s'", stmt->as.assign_stmt.name);
            }
            check_expr(stmt->as.assign_stmt.value, scope, diag);
            break;
        case STMT_EXPR:
        case STMT_RETURN:
            check_expr(stmt->as.expr, scope, diag);
            break;
        case STMT_FOR_EACH: {
            check_expr(stmt->as.for_each.array, scope, diag);
            Scope child = {0};
            child.parent = scope;
            scope_define(&child, stmt->as.for_each.var);
            check_block(stmt->as.for_each.body, &child, diag, 0);
            scope_free_names(&child);
            break;
        }
        case STMT_BLOCK:
            check_block(stmt->as.block, scope, diag, 1);
            break;
    }
}

static void check_block(Block *block, Scope *parent, Diagnostic *diag, int create_scope) {
    Scope child = {0};
    Scope *scope = parent;
    if (create_scope) {
        child.parent = parent;
        scope = &child;
    }
    for (int i = 0; i < block->count; i++) {
        check_stmt(block->items[i], scope, diag);
    }
    if (create_scope) {
        scope_free_names(&child);
    }
}

static void check_expr(Expr *expr, Scope *scope, Diagnostic *diag) {
    if (!expr) {
        return;
    }
    switch (expr->kind) {
        case EXPR_VAR:
            if (!scope_find(scope, expr->as.var_name)) {
                diag_error(diag, expr->line, 1, "语义检查", "未定义变量 '%s'", expr->as.var_name);
            }
            break;
        case EXPR_SOME:
            check_expr(expr->as.some_value, scope, diag);
            break;
        case EXPR_ARRAY:
            for (int i = 0; i < expr->as.array_items.count; i++) {
                check_expr(expr->as.array_items.items[i], scope, diag);
            }
            break;
        case EXPR_CALL:
            check_expr(expr->as.call.callee, scope, diag);
            for (int i = 0; i < expr->as.call.args.count; i++) {
                check_expr(expr->as.call.args.items[i], scope, diag);
            }
            break;
        case EXPR_INDEX:
            check_expr(expr->as.index.array, scope, diag);
            check_expr(expr->as.index.index, scope, diag);
            break;
        case EXPR_BINARY:
            check_expr(expr->as.binary.left, scope, diag);
            check_expr(expr->as.binary.right, scope, diag);
            break;
        case EXPR_UNARY:
            check_expr(expr->as.unary.expr, scope, diag);
            break;
        case EXPR_FN: {
            Scope fn_scope = {0};
            fn_scope.parent = scope;
            for (int i = 0; i < expr->as.fn.params.count; i++) {
                scope_define(&fn_scope, expr->as.fn.params.items[i]);
            }
            check_block(expr->as.fn.body, &fn_scope, diag, 0);
            scope_free_names(&fn_scope);
            break;
        }
        case EXPR_IF:
            check_expr(expr->as.if_expr.condition, scope, diag);
            check_block(expr->as.if_expr.then_block, scope, diag, 1);
            if (expr->as.if_expr.else_block) {
                check_block(expr->as.if_expr.else_block, scope, diag, 1);
            }
            break;
        case EXPR_MATCH:
            check_expr(expr->as.match_expr.target, scope, diag);
            for (int i = 0; i < expr->as.match_expr.arms.count; i++) {
                MatchArm *arm = &expr->as.match_expr.arms.items[i];
                Scope arm_scope = {0};
                arm_scope.parent = scope;
                if (arm->is_some && arm->binding) {
                    scope_define(&arm_scope, arm->binding);
                }
                check_expr(arm->guard, &arm_scope, diag);
                check_expr(arm->value, &arm_scope, diag);
                scope_free_names(&arm_scope);
            }
            break;
        case EXPR_INT:
        case EXPR_BOOL:
        case EXPR_NONE:
            break;
    }
}

int semantic_check(Program *program, Diagnostic *diag) {
    Scope root = {0};
    check_block(program->body, &root, diag, 0);
    scope_free_names(&root);
    return diag->error_count == 0;
}
