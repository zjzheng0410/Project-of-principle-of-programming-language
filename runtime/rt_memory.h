#ifndef PPL_RT_MEMORY_H
#define PPL_RT_MEMORY_H

#include <stddef.h>

void *ppl_xmalloc(size_t size);
void *ppl_xcalloc(size_t count, size_t size);

#endif
