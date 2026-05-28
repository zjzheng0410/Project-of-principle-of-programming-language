#include "text_buffer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tb_init(TextBuffer *tb) {
    tb->capacity = 4096;
    tb->length = 0;
    tb->data = (char *)malloc(tb->capacity);
    if (!tb->data) {
        return 0;
    }
    tb->data[0] = '\0';
    return 1;
}

static int tb_reserve(TextBuffer *tb, size_t extra) {
    size_t need = tb->length + extra + 1;
    if (need <= tb->capacity) {
        return 1;
    }
    size_t cap = tb->capacity;
    while (cap < need) {
        cap *= 2;
    }
    char *data = (char *)realloc(tb->data, cap);
    if (!data) {
        return 0;
    }
    tb->data = data;
    tb->capacity = cap;
    return 1;
}

int tb_append(TextBuffer *tb, const char *text) {
    size_t len = strlen(text);
    if (!tb_reserve(tb, len)) {
        return 0;
    }
    memcpy(tb->data + tb->length, text, len + 1);
    tb->length += len;
    return 1;
}

int tb_appendf(TextBuffer *tb, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    va_list copy;
    va_copy(copy, ap);
    int len = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    if (len < 0) {
        va_end(ap);
        return 0;
    }
    if (!tb_reserve(tb, (size_t)len)) {
        va_end(ap);
        return 0;
    }
    vsnprintf(tb->data + tb->length, (size_t)len + 1, fmt, ap);
    va_end(ap);
    tb->length += (size_t)len;
    return 1;
}

void tb_free(TextBuffer *tb) {
    free(tb->data);
    tb->data = NULL;
    tb->length = 0;
    tb->capacity = 0;
}
