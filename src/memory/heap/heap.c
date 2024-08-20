#include "heap.h"
#include "../../kernel.h"
#include <stdbool.h>

/* returns true if ptr is aligned with the heap block size */
static bool heap_validate_alignment(void *ptr)
{
    return ((unsigned int)ptr % SIMPOS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table)
{
    int res = 0;
    return 0;
}