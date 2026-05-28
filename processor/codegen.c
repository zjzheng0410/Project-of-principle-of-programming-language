#include "codegen.h"

#include "text_buffer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct EmitCtx {
    TextBuffer *out;
    int indent;
    int temp_id;
    int scope_id;
    int print_expect;
} EmitCtx;

static char *dup_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    va_list copy;
    va_copy(copy, ap);
    int len = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    if (len < 0) {
        va_end(ap);
        return NULL;
    }
    char *buf = (char *)malloc((size_t)len + 1);
    if (!buf) {
        va_end(ap);
        return NULL;
    }
    vsnprintf(buf, (size_t)len + 1, fmt, ap);
    va_end(ap);
    return buf;
}

static char *c_string(const char *text) {
    TextBuffer tb;
    tb_init(&tb);
    tb_append(&tb, "\"");
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        switch (*p) {
            case '\\': tb_append(&tb, "\\\\"); break;
            case '"': tb_append(&tb, "\\\""); break;
            case '\n': tb_append(&tb, "\\n"); break;
            case '\r': tb_append(&tb, "\\r"); break;
            case '\t': tb_append(&tb, "\\t"); break;
            default:
                if (*p < 32 || *p > 126) {
                    tb_appendf(&tb, "\\x%02x", *p);
                } else {
                    char ch[2] = {(char)*p, '\0'};
                    tb_append(&tb, ch);
                }
                break;
        }
    }
    tb_append(&tb, "\"");
    return tb.data;
}

static void emit_indent(EmitCtx *ctx) {
    for (int i = 0; i < ctx->indent; i++) {
        tb_append(ctx->out, "    ");
    }
}

static void emit_line(EmitCtx *ctx, const char *fmt, ...) {
    emit_indent(ctx);
    va_list ap;
    va_start(ap, fmt);
    va_list copy;
    va_copy(copy, ap);
    int len = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    char *buf = (char *)malloc((size_t)len + 1);
    vsnprintf(buf, (size_t)len + 1, fmt, ap);
    va_end(ap);
    tb_append(ctx->out, buf);
    tb_append(ctx->out, "\n");
    free(buf);
}

static char *new_temp(EmitCtx *ctx) {
    return dup_printf("_v%d", ++ctx->temp_id);
}

static void emit_stmt(EmitCtx *ctx, Stmt *stmt, int as_function_body_last);
static char *emit_expr(EmitCtx *ctx, Expr *expr);
static void emit_expect_print(EmitCtx *ctx, Stmt *stmt, const char *value_name);

static int is_last_expr(Block *block, int index) {
    return index == block->count - 1 && block->items[index]->kind == STMT_EXPR;
}

static char *emit_block_value(EmitCtx *ctx, Block *block, int create_scope) {
    char *result = NULL;
    char *prev_name = NULL;
    int scope_id = 0;
    if (create_scope) {
        scope_id = ++ctx->scope_id;
        prev_name = dup_printf("_prev_env%d", scope_id);
        emit_line(ctx, "PPLEnv *%s = env;", prev_name);
        emit_line(ctx, "env = ppl_env_new(env);");
    }
    for (int i = 0; i < block->count; i++) {
        if (is_last_expr(block, i)) {
            result = emit_expr(ctx, block->items[i]->as.expr);
            emit_expect_print(ctx, block->items[i], result);
        } else {
            emit_stmt(ctx, block->items[i], 0);
        }
    }
    if (!result) {
        result = new_temp(ctx);
        emit_line(ctx, "PPLValue %s = ppl_none();", result);
    }
    if (create_scope) {
        emit_line(ctx, "env = %s;", prev_name);
        free(prev_name);
    }
    return result;
}

static void emit_expect_print(EmitCtx *ctx, Stmt *stmt, const char *value_name) {
    if (!ctx->print_expect || !stmt->expect) {
        return;
    }
    emit_line(ctx, "ppl_print_value(%s);", value_name);
    emit_line(ctx, "printf(\"\\n\");");
}

static void emit_stmt(EmitCtx *ctx, Stmt *stmt, int as_function_body_last) {
    switch (stmt->kind) {
        case STMT_LET: {
            char *value = emit_expr(ctx, stmt->as.let_stmt.value);
            char *name = c_string(stmt->as.let_stmt.name);
            emit_line(ctx, "ppl_env_define(env, %s, %s);", name, value);
            emit_expect_print(ctx, stmt, value);
            if (as_function_body_last) {
                emit_line(ctx, "return %s;", value);
            }
            free(name);
            free(value);
            break;
        }
        case STMT_ASSIGN: {
            char *value = emit_expr(ctx, stmt->as.assign_stmt.value);
            char *name = c_string(stmt->as.assign_stmt.name);
            emit_line(ctx, "ppl_env_assign(env, %s, %s);", name, value);
            emit_expect_print(ctx, stmt, value);
            if (as_function_body_last) {
                emit_line(ctx, "return %s;", value);
            }
            free(name);
            free(value);
            break;
        }
        case STMT_EXPR: {
            char *value = emit_expr(ctx, stmt->as.expr);
            emit_expect_print(ctx, stmt, value);
            if (as_function_body_last) {
                emit_line(ctx, "return %s;", value);
            } else if (!stmt->expect) {
                emit_line(ctx, "(void)%s;", value);
            }
            free(value);
            break;
        }
        case STMT_RETURN: {
            char *value = emit_expr(ctx, stmt->as.expr);
            emit_line(ctx, "return %s;", value);
            free(value);
            break;
        }
        case STMT_FOR_EACH: {
            char *array = emit_expr(ctx, stmt->as.for_each.array);
            int id = ++ctx->scope_id;
            char *loop_var = c_string(stmt->as.for_each.var);
            emit_line(ctx, "for (size_t _i%d = 0; _i%d < ppl_array_len(%s); _i%d++) {", id, id, array, id);
            ctx->indent++;
            emit_line(ctx, "PPLEnv *_prev_env%d = env;", id);
            emit_line(ctx, "env = ppl_env_new(env);");
            emit_line(ctx, "ppl_env_define(env, %s, ppl_array_get(%s, _i%d));", loop_var, array, id);
            for (int i = 0; i < stmt->as.for_each.body->count; i++) {
                emit_stmt(ctx, stmt->as.for_each.body->items[i], 0);
            }
            emit_line(ctx, "env = _prev_env%d;", id);
            ctx->indent--;
            emit_line(ctx, "}");
            free(loop_var);
            free(array);
            break;
        }
        case STMT_BLOCK: {
            char *value = emit_block_value(ctx, stmt->as.block, 1);
            emit_expect_print(ctx, stmt, value);
            if (as_function_body_last) {
                emit_line(ctx, "return %s;", value);
            } else if (!stmt->expect) {
                emit_line(ctx, "(void)%s;", value);
            }
            free(value);
            break;
        }
    }
}

static char *emit_args(EmitCtx *ctx, ExprList *args, char **args_name_out) {
    char *args_name = dup_printf("_args%d", ++ctx->temp_id);
    if (args->count == 0) {
        emit_line(ctx, "PPLValue *%s = NULL;", args_name);
        *args_name_out = args_name;
        return NULL;
    }
    char **values = (char **)calloc((size_t)args->count, sizeof(char *));
    for (int i = 0; i < args->count; i++) {
        values[i] = emit_expr(ctx, args->items[i]);
    }
    emit_indent(ctx);
    tb_appendf(ctx->out, "PPLValue %s[%d] = {", args_name, args->count);
    for (int i = 0; i < args->count; i++) {
        if (i > 0) {
            tb_append(ctx->out, ", ");
        }
        tb_append(ctx->out, values[i]);
        free(values[i]);
    }
    tb_append(ctx->out, "};\n");
    free(values);
    *args_name_out = args_name;
    return NULL;
}

static char *emit_expr(EmitCtx *ctx, Expr *expr) {
    char *result = new_temp(ctx);
    switch (expr->kind) {
        case EXPR_INT:
            emit_line(ctx, "PPLValue %s = ppl_int(%ld);", result, expr->as.int_value);
            break;
        case EXPR_BOOL:
            emit_line(ctx, "PPLValue %s = ppl_bool(%d);", result, expr->as.bool_value);
            break;
        case EXPR_VAR: {
            char *name = c_string(expr->as.var_name);
            emit_line(ctx, "PPLValue %s = ppl_env_get(env, %s);", result, name);
            free(name);
            break;
        }
        case EXPR_NONE:
            emit_line(ctx, "PPLValue %s = ppl_none();", result);
            break;
        case EXPR_SOME: {
            char *inner = emit_expr(ctx, expr->as.some_value);
            emit_line(ctx, "PPLValue %s = ppl_some(%s);", result, inner);
            free(inner);
            break;
        }
        case EXPR_ARRAY: {
            if (expr->as.array_items.count == 0) {
                emit_line(ctx, "PPLValue %s = ppl_array(0, NULL);", result);
            } else {
                ExprList *items = &expr->as.array_items;
                char **values = (char **)calloc((size_t)items->count, sizeof(char *));
                for (int i = 0; i < items->count; i++) {
                    values[i] = emit_expr(ctx, items->items[i]);
                }
                char *arr_name = dup_printf("_items%d", ++ctx->temp_id);
                emit_indent(ctx);
                tb_appendf(ctx->out, "PPLValue %s[%d] = {", arr_name, items->count);
                for (int i = 0; i < items->count; i++) {
                    if (i > 0) {
                        tb_append(ctx->out, ", ");
                    }
                    tb_append(ctx->out, values[i]);
                    free(values[i]);
                }
                tb_append(ctx->out, "};\n");
                emit_line(ctx, "PPLValue %s = ppl_array(%d, %s);", result, items->count, arr_name);
                free(arr_name);
                free(values);
            }
            break;
        }
        case EXPR_CALL: {
            char *args_name = NULL;
            emit_args(ctx, &expr->as.call.args, &args_name);
            if (expr->as.call.callee->kind == EXPR_VAR) {
                char *name = c_string(expr->as.call.callee->as.var_name);
                emit_line(ctx, "PPLValue %s = ppl_call_name(env, %s, %d, %s);", result, name, expr->as.call.args.count, args_name);
                free(name);
            } else {
                char *callee = emit_expr(ctx, expr->as.call.callee);
                emit_line(ctx, "PPLValue %s = ppl_call_value(%s, %d, %s);", result, callee, expr->as.call.args.count, args_name);
                free(callee);
            }
            free(args_name);
            break;
        }
        case EXPR_INDEX: {
            char *arr = emit_expr(ctx, expr->as.index.array);
            char *idx = emit_expr(ctx, expr->as.index.index);
            emit_line(ctx, "PPLValue %s = ppl_index(%s, %s);", result, arr, idx);
            free(arr);
            free(idx);
            break;
        }
        case EXPR_BINARY: {
            char *left = emit_expr(ctx, expr->as.binary.left);
            char *right = emit_expr(ctx, expr->as.binary.right);
            char *op = c_string(expr->as.binary.op);
            emit_line(ctx, "PPLValue %s = ppl_binary(%s, %s, %s);", result, op, left, right);
            free(op);
            free(left);
            free(right);
            break;
        }
        case EXPR_UNARY: {
            char *inner = emit_expr(ctx, expr->as.unary.expr);
            char *op = c_string(expr->as.unary.op);
            emit_line(ctx, "PPLValue %s = ppl_unary(%s, %s);", result, op, inner);
            free(op);
            free(inner);
            break;
        }
        case EXPR_FN:
            emit_line(ctx, "PPLValue %s = ppl_make_function(\"fn_%d\", ppl_fn_%d, env);", result, expr->as.fn.fn_id, expr->as.fn.fn_id);
            break;
        case EXPR_IF: {
            char *cond = emit_expr(ctx, expr->as.if_expr.condition);
            emit_line(ctx, "PPLValue %s;", result);
            emit_line(ctx, "if (ppl_truthy(%s)) {", cond);
            ctx->indent++;
            char *then_value = emit_block_value(ctx, expr->as.if_expr.then_block, 1);
            emit_line(ctx, "%s = %s;", result, then_value);
            free(then_value);
            ctx->indent--;
            emit_line(ctx, "} else {");
            ctx->indent++;
            if (expr->as.if_expr.else_block) {
                char *else_value = emit_block_value(ctx, expr->as.if_expr.else_block, 1);
                emit_line(ctx, "%s = %s;", result, else_value);
                free(else_value);
            } else {
                emit_line(ctx, "%s = ppl_none();", result);
            }
            ctx->indent--;
            emit_line(ctx, "}");
            free(cond);
            break;
        }
        case EXPR_MATCH: {
            char *target = emit_expr(ctx, expr->as.match_expr.target);
            int id = ++ctx->scope_id;
            emit_line(ctx, "PPLValue %s;", result);
            emit_line(ctx, "int _matched%d = 0;", id);
            for (int i = 0; i < expr->as.match_expr.arms.count; i++) {
                MatchArm *arm = &expr->as.match_expr.arms.items[i];
                if (arm->is_some) {
                    emit_line(ctx, "if (!_matched%d && ppl_is_some_value(%s)) {", id, target);
                } else {
                    emit_line(ctx, "if (!_matched%d && ppl_is_none_value(%s)) {", id, target);
                }
                ctx->indent++;
                emit_line(ctx, "PPLEnv *_prev_env%d_%d = env;", id, i);
                emit_line(ctx, "env = ppl_env_new(env);");
                if (arm->is_some) {
                    char *binding = c_string(arm->binding);
                    emit_line(ctx, "ppl_env_define(env, %s, ppl_unwrap_some(%s));", binding, target);
                    free(binding);
                }
                if (arm->guard) {
                    char *guard = emit_expr(ctx, arm->guard);
                    emit_line(ctx, "if (ppl_truthy(%s)) {", guard);
                    ctx->indent++;
                    char *value = emit_expr(ctx, arm->value);
                    emit_line(ctx, "%s = %s;", result, value);
                    emit_line(ctx, "_matched%d = 1;", id);
                    free(value);
                    ctx->indent--;
                    emit_line(ctx, "}");
                    free(guard);
                } else {
                    char *value = emit_expr(ctx, arm->value);
                    emit_line(ctx, "%s = %s;", result, value);
                    emit_line(ctx, "_matched%d = 1;", id);
                    free(value);
                }
                emit_line(ctx, "env = _prev_env%d_%d;", id, i);
                ctx->indent--;
                emit_line(ctx, "}");
            }
            emit_line(ctx, "if (!_matched%d) { ppl_runtime_error(\"match\", \"没有匹配分支\"); }", id);
            free(target);
            break;
        }
    }
    return result;
}

static void walk_expr_for_fns(EmitCtx *ctx, Expr *expr);

static void walk_block_for_fns(EmitCtx *ctx, Block *block) {
    for (int i = 0; i < block->count; i++) {
        Stmt *stmt = block->items[i];
        switch (stmt->kind) {
            case STMT_LET:
                walk_expr_for_fns(ctx, stmt->as.let_stmt.value);
                break;
            case STMT_ASSIGN:
                walk_expr_for_fns(ctx, stmt->as.assign_stmt.value);
                break;
            case STMT_EXPR:
            case STMT_RETURN:
                walk_expr_for_fns(ctx, stmt->as.expr);
                break;
            case STMT_FOR_EACH:
                walk_expr_for_fns(ctx, stmt->as.for_each.array);
                walk_block_for_fns(ctx, stmt->as.for_each.body);
                break;
            case STMT_BLOCK:
                walk_block_for_fns(ctx, stmt->as.block);
                break;
        }
    }
}

static void emit_fn_prototype(EmitCtx *ctx, Expr *expr) {
    emit_line(ctx, "static PPLValue ppl_fn_%d(PPLFunc *self, int argc, PPLValue *args);", expr->as.fn.fn_id);
}

static void walk_expr_for_fns(EmitCtx *ctx, Expr *expr) {
    if (!expr) {
        return;
    }
    switch (expr->kind) {
        case EXPR_FN:
            emit_fn_prototype(ctx, expr);
            walk_block_for_fns(ctx, expr->as.fn.body);
            break;
        case EXPR_SOME:
            walk_expr_for_fns(ctx, expr->as.some_value);
            break;
        case EXPR_ARRAY:
            for (int i = 0; i < expr->as.array_items.count; i++) walk_expr_for_fns(ctx, expr->as.array_items.items[i]);
            break;
        case EXPR_CALL:
            walk_expr_for_fns(ctx, expr->as.call.callee);
            for (int i = 0; i < expr->as.call.args.count; i++) walk_expr_for_fns(ctx, expr->as.call.args.items[i]);
            break;
        case EXPR_INDEX:
            walk_expr_for_fns(ctx, expr->as.index.array);
            walk_expr_for_fns(ctx, expr->as.index.index);
            break;
        case EXPR_BINARY:
            walk_expr_for_fns(ctx, expr->as.binary.left);
            walk_expr_for_fns(ctx, expr->as.binary.right);
            break;
        case EXPR_UNARY:
            walk_expr_for_fns(ctx, expr->as.unary.expr);
            break;
        case EXPR_IF:
            walk_expr_for_fns(ctx, expr->as.if_expr.condition);
            walk_block_for_fns(ctx, expr->as.if_expr.then_block);
            if (expr->as.if_expr.else_block) walk_block_for_fns(ctx, expr->as.if_expr.else_block);
            break;
        case EXPR_MATCH:
            walk_expr_for_fns(ctx, expr->as.match_expr.target);
            for (int i = 0; i < expr->as.match_expr.arms.count; i++) {
                walk_expr_for_fns(ctx, expr->as.match_expr.arms.items[i].guard);
                walk_expr_for_fns(ctx, expr->as.match_expr.arms.items[i].value);
            }
            break;
        case EXPR_INT:
        case EXPR_BOOL:
        case EXPR_VAR:
        case EXPR_NONE:
            break;
    }
}

static void emit_function_defs_expr(EmitCtx *ctx, Expr *expr);

static void emit_function_body(EmitCtx *ctx, Expr *fn) {
    emit_line(ctx, "static PPLValue ppl_fn_%d(PPLFunc *self, int argc, PPLValue *args) {", fn->as.fn.fn_id);
    ctx->indent++;
    emit_line(ctx, "ppl_expect_argc(\"fn_%d\", argc, %d);", fn->as.fn.fn_id, fn->as.fn.params.count);
    if (fn->as.fn.params.count == 0) {
        emit_line(ctx, "(void)args;");
    }
    emit_line(ctx, "PPLEnv *env = ppl_env_new(self->env);");
    for (int i = 0; i < fn->as.fn.params.count; i++) {
        char *param = c_string(fn->as.fn.params.items[i]);
        emit_line(ctx, "ppl_env_define(env, %s, args[%d]);", param, i);
        free(param);
    }
    int old_print = ctx->print_expect;
    ctx->print_expect = 0;
    for (int i = 0; i < fn->as.fn.body->count; i++) {
        emit_stmt(ctx, fn->as.fn.body->items[i], i == fn->as.fn.body->count - 1);
    }
    ctx->print_expect = old_print;
    emit_line(ctx, "return ppl_none();");
    ctx->indent--;
    emit_line(ctx, "}");
    emit_line(ctx, "");
}

static void emit_function_defs_block(EmitCtx *ctx, Block *block) {
    for (int i = 0; i < block->count; i++) {
        Stmt *stmt = block->items[i];
        switch (stmt->kind) {
            case STMT_LET:
                emit_function_defs_expr(ctx, stmt->as.let_stmt.value);
                break;
            case STMT_ASSIGN:
                emit_function_defs_expr(ctx, stmt->as.assign_stmt.value);
                break;
            case STMT_EXPR:
            case STMT_RETURN:
                emit_function_defs_expr(ctx, stmt->as.expr);
                break;
            case STMT_FOR_EACH:
                emit_function_defs_expr(ctx, stmt->as.for_each.array);
                emit_function_defs_block(ctx, stmt->as.for_each.body);
                break;
            case STMT_BLOCK:
                emit_function_defs_block(ctx, stmt->as.block);
                break;
        }
    }
}

static void emit_function_defs_expr(EmitCtx *ctx, Expr *expr) {
    if (!expr) {
        return;
    }
    switch (expr->kind) {
        case EXPR_FN:
            emit_function_defs_block(ctx, expr->as.fn.body);
            emit_function_body(ctx, expr);
            break;
        case EXPR_SOME:
            emit_function_defs_expr(ctx, expr->as.some_value);
            break;
        case EXPR_ARRAY:
            for (int i = 0; i < expr->as.array_items.count; i++) emit_function_defs_expr(ctx, expr->as.array_items.items[i]);
            break;
        case EXPR_CALL:
            emit_function_defs_expr(ctx, expr->as.call.callee);
            for (int i = 0; i < expr->as.call.args.count; i++) emit_function_defs_expr(ctx, expr->as.call.args.items[i]);
            break;
        case EXPR_INDEX:
            emit_function_defs_expr(ctx, expr->as.index.array);
            emit_function_defs_expr(ctx, expr->as.index.index);
            break;
        case EXPR_BINARY:
            emit_function_defs_expr(ctx, expr->as.binary.left);
            emit_function_defs_expr(ctx, expr->as.binary.right);
            break;
        case EXPR_UNARY:
            emit_function_defs_expr(ctx, expr->as.unary.expr);
            break;
        case EXPR_IF:
            emit_function_defs_expr(ctx, expr->as.if_expr.condition);
            emit_function_defs_block(ctx, expr->as.if_expr.then_block);
            if (expr->as.if_expr.else_block) emit_function_defs_block(ctx, expr->as.if_expr.else_block);
            break;
        case EXPR_MATCH:
            emit_function_defs_expr(ctx, expr->as.match_expr.target);
            for (int i = 0; i < expr->as.match_expr.arms.count; i++) {
                emit_function_defs_expr(ctx, expr->as.match_expr.arms.items[i].guard);
                emit_function_defs_expr(ctx, expr->as.match_expr.arms.items[i].value);
            }
            break;
        case EXPR_INT:
        case EXPR_BOOL:
        case EXPR_VAR:
        case EXPR_NONE:
            break;
    }
}

int codegen_write_c(Program *program, const char *output_path, Diagnostic *diag) {
    TextBuffer tb;
    if (!tb_init(&tb)) {
        diag_error(diag, 1, 1, "代码生成", "内存不足");
        return 0;
    }
    EmitCtx ctx = {&tb, 0, 0, 0, 1};
    tb_append(&tb, "#include <stdio.h>\n");
    tb_append(&tb, "#include \"ppl_runtime.h\"\n\n");
    walk_block_for_fns(&ctx, program->body);
    tb_append(&tb, "\nint main(void) {\n");
    ctx.indent = 1;
    emit_line(&ctx, "PPLEnv *env = ppl_env_new(NULL);");
    for (int i = 0; i < program->body->count; i++) {
        emit_stmt(&ctx, program->body->items[i], 0);
    }
    emit_line(&ctx, "return 0;");
    ctx.indent = 0;
    tb_append(&tb, "}\n\n");
    emit_function_defs_block(&ctx, program->body);

    FILE *fp = fopen(output_path, "wb");
    if (!fp) {
        diag_error(diag, 1, 1, "代码生成", "无法写入输出文件 '%s'", output_path);
        tb_free(&tb);
        return 0;
    }
    size_t wrote = fwrite(tb.data, 1, tb.length, fp);
    fclose(fp);
    if (wrote != tb.length) {
        diag_error(diag, 1, 1, "代码生成", "输出文件写入不完整 '%s'", output_path);
        tb_free(&tb);
        return 0;
    }
    tb_free(&tb);
    return 1;
}
