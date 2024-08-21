#include "kernel_heap.h"
#include "heap.h"
#include "config.h"
#include "../../kernel.h"

/* defined structures */
struct heap kernel_heap;
struct heap_table kernel_heap_table;

/* initialize the heap and heap table */
void kernel_heap_init()
{
    int total_table_entries = SIMPOS_HEAP_SIZE_BYTES / SIMPOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY *)SIMPOS_HEAP_TABLE_ADDRESS;
    kernel_heap_table.total = total_table_entries;

    void *end = (void *)(SIMPOS_HEAP_ADDRESS + SIMPOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void *)SIMPOS_HEAP_ADDRESS, end, &kernel_heap_table);

    if (res < 0)
    {
        print("Failed to create heap\n");
    }
}

/* allocate memory */
void *kernel_malloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

/* free allocated memory */
void kernel_free(void *ptr)
{
    heap_free(&kernel_heap, ptr);
}