#ifndef KERNEL_HEAP
#define KERNEL_HEAP
#include <stdint.h>
#include <stddef.h>

/* function prototypes */
void kernel_heap_init();
void *kmalloc(size_t size);
void kernel_free(void *ptr);
void *kzalloc(size_t size);

#endif