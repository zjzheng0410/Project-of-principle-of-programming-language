#include "cli.h"
#include "compile_pipeline.h"

int main(int argc, char **argv) {
    CliOptions options;
    if (!cli_parse(argc, argv, &options)) {
        cli_print_usage(argv[0]);
        return 2;
    }
    return compile_pipeline_run(&options);
}
