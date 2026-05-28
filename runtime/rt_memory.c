#include "rt_memory.h"

#include "ppl_runtime.h"

#include <stdlib.h>

void *ppl_xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        ppl_runtime_error("运行时内存", "内存申请失败");
    }
    return ptr;
}

void *ppl_xcalloc(size_t count, size_t size) {
    void *ptr = calloc(count, size);
    if (!ptr) {
        ppl_runtime_error("运行时内存", "内存申请失败");
    }
    return ptr;
}
