#ifndef PPL_TEXT_BUFFER_H
#define PPL_TEXT_BUFFER_H

#include <stddef.h>

typedef struct TextBuffer {
    char *data;
    size_t length;
    size_t capacity;
} TextBuffer;

int tb_init(TextBuffer *tb);
int tb_append(TextBuffer *tb, const char *text);
int tb_appendf(TextBuffer *tb, const char *fmt, ...);
void tb_free(TextBuffer *tb);

#endif
