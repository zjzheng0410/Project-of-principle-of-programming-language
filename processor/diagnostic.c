#include "diagnostic.h"

#include <stdarg.h>
#include <stdio.h>

void diag_init(Diagnostic *diag, const char *path) {
    diag->path = path;
    diag->error_count = 0;
}

void diag_error(Diagnostic *diag, int line, int column, const char *stage, const char *fmt, ...) {
    diag->error_count++;
    fprintf(stderr, "%s:%d:%d: %s: ", diag->path ? diag->path : "<input>", line, column, stage);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
}
