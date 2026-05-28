#include "compile_pipeline.h"

#include "codegen.h"
#include "diagnostic.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "source.h"

#include <stdio.h>
#include <stdlib.h>

static void dump_tokens(TokenVec *tokens) {
    for (int i = 0; i < tokens->count; i++) {
        Token *t = &tokens->items[i];
        if (t->text) {
            printf("%d:%d %-14s %s\n", t->line, t->column, token_kind_name(t->kind), t->text);
        } else {
            printf("%d:%d %-14s\n", t->line, t->column, token_kind_name(t->kind));
        }
    }
}

int compile_pipeline_run(const CliOptions *options) {
    char *read_error = NULL;
    char *source = source_read_file(options->input_path, &read_error);
    if (!source) {
        fprintf(stderr, "%s\n", read_error ? read_error : "源码读取：未知错误");
        free(read_error);
        return 1;
    }

    Diagnostic diag;
    diag_init(&diag, options->input_path);
    TokenVec tokens;
    if (!lex_source(source, &diag, &tokens)) {
        fprintf(stderr, "词法分析：内存不足\n");
        free(source);
        return 1;
    }
    if (options->dump_tokens) {
        dump_tokens(&tokens);
    }
    if (diag.error_count > 0) {
        token_vec_free(&tokens);
        free(source);
        return 1;
    }

    Program *program = parse_program(&tokens, &diag);
    if (!program || diag.error_count > 0) {
        token_vec_free(&tokens);
        free(source);
        return 1;
    }
    if (!semantic_check(program, &diag)) {
        program_free(program);
        token_vec_free(&tokens);
        free(source);
        return 1;
    }
    int ok = codegen_write_c(program, options->output_path, &diag);
    program_free(program);
    token_vec_free(&tokens);
    free(source);
    return ok ? 0 : 1;
}
