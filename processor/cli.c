#include "cli.h"

#include <stdio.h>
#include <string.h>

void cli_print_usage(const char *program) {
    fprintf(stderr, "用法: %s <input.ppl> -o <output.c> [--dump-tokens]\n", program);
}

int cli_parse(int argc, char **argv, CliOptions *options) {
    options->input_path = NULL;
    options->output_path = NULL;
    options->dump_tokens = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "命令行：-o 后缺少输出路径\n");
                return 0;
            }
            options->output_path = argv[++i];
        } else if (strcmp(argv[i], "--dump-tokens") == 0) {
            options->dump_tokens = 1;
        } else if (!options->input_path) {
            options->input_path = argv[i];
        } else {
            fprintf(stderr, "命令行：未知参数 '%s'\n", argv[i]);
            return 0;
        }
    }
    if (!options->input_path || !options->output_path) {
        return 0;
    }
    return 1;
}
