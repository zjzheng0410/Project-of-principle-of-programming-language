#ifndef PPL_CLI_H
#define PPL_CLI_H

typedef struct CliOptions {
    const char *input_path;
    const char *output_path;
    int dump_tokens;
} CliOptions;

int cli_parse(int argc, char **argv, CliOptions *options);
void cli_print_usage(const char *program);

#endif
