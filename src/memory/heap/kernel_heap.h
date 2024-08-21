#ifndef KERNEL_HEAP
#define KERNEL_HEAP
#include <stdint.h>
#include <stddef.h>

/* function prototypes */
void kernel_heap_init();
void *kernel_malloc(size_t size);
void kernel_free(void *ptr);

#endif