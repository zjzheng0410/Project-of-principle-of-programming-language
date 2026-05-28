#include "ppl_runtime.h"

#include <stdio.h>
#include <stdlib.h>

void ppl_runtime_error(const char *stage, const char *message) {
    fprintf(stderr, "%s: %s\n", stage, message);
    exit(1);
}
