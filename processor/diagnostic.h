#ifndef PPL_DIAGNOSTIC_H
#define PPL_DIAGNOSTIC_H

typedef struct Diagnostic {
    const char *path;
    int error_count;
} Diagnostic;

void diag_init(Diagnostic *diag, const char *path);
void diag_error(Diagnostic *diag, int line, int column, const char *stage, const char *fmt, ...);

#endif
