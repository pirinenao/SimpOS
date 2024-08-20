#ifndef KERNEL_HEAP
#define KERNEL_HEAP

#include <stdint.h>
#include <stddef.h>
void kernel_heap_init();
void *kernel_malloc(size_t size);

#endif