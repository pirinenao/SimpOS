#include "heap.h"
#include "../../kernel.h"
#include "../../status.h"
#include "../memory.h"

#include <stdbool.h>

/* check if the table size is valid */
static int heap_validate_table(void *ptr, void *end, struct heap_table *table)
{
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / SIMPOS_HEAP_BLOCK_SIZE;

    if (table->total != total_blocks)
    {
        res = -EINVARG;
        return res;
    }

    return res;
}

/* returns true if ptr is aligned with the heap block size */
static bool heap_validate_alignment(void *ptr)
{
    return ((unsigned int)ptr % SIMPOS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table)
{
    int res = 0;

    /* check if the give values are aligned */
    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end))
    {
        res = -EINVARG;
        return res;
    }

    /* initialize heap */
    memset(heap, 0, sizeof(struct heap));

    heap->start_address = ptr;
    heap->table = table;

    /* validate table size */
    res = heap_validate_table(ptr, end, table);

    if (res < 0)
    {
        return res;
    }

    /* initialize heap table */
    size_t table_size = sizeof(SIMPOS_HEAP_BLOCK_SIZE) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return res;
}

/* aligns the value */
static uint32_t heap_align_value_to_upper(uint32_t val)
{
    if ((val % SIMPOS_HEAP_BLOCK_SIZE) == 0)
    {
        return val;
    }

    val = (val - (val % SIMPOS_HEAP_BLOCK_SIZE));
    val += SIMPOS_HEAP_BLOCK_SIZE;

    return val;
}

/* returns the lowest bits of the entry */
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f;
}

/* finds the block to start the memory allocation from */
int heap_get_start_block(struct heap *heap, uint32_t total_blocks)
{
    struct heap_table *table = heap->table;
    int block_count = 0;
    int starting_block = -1;

    /* loop through the table */
    for (size_t i = 0; i < table->total; i++)
    {
        /* if entry type is not free, continue to the next entry */
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE)
        {
            block_count = 0;
            starting_block = -1;
            continue;
        }

        if (starting_block == -1)
        {
            starting_block = i;
        }

        block_count++;

        /* if enough free blocks */
        if (block_count == total_blocks)
        {
            break;
        }
    }

    if (starting_block == -1)
    {
        return -ENOMEM;
    }

    return starting_block;
}

/* finds the address of the starting block */
void *heap_block_to_address(struct heap *heap, int block)
{
    return heap->start_address + (block * SIMPOS_HEAP_BLOCK_SIZE);
}

/* marks all the blocks taken */
void heap_mark_blocks_taken(struct heap *heap, int start_block, int total_blocks)
{
    int end_block = (start_block + total_blocks) - 1;

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;

    if (total_blocks > 1)
    {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

/* allocates the heap memory blocks */
void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks)
{
    void *address = 0;
    /* find the starting block for the memory allocation */
    int start_block = heap_get_start_block(heap, total_blocks);

    if (start_block < 0)
    {
        return address;
    }

    /* find the memory address for the starting block */
    address = heap_block_to_address(heap, start_block);

    /* mark all the blocks needed for the memory allocation as taken */
    heap_mark_blocks_taken(heap, start_block, total_blocks);

    return address;
}

void *heap_malloc(struct heap *heap, size_t size)
{
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / SIMPOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap *heap, void *ptr)
{
    return;
}