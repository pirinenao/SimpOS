#include "heap.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"
#include <stdbool.h>

/* validate the table size */
static int heap_validate_table(void *ptr, void *end, struct heap_table *table)
{
    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / SIMPOS_HEAP_BLOCK_SIZE;

    /* if the table sizes doesn't match */
    if (table->total != total_blocks)
    {
        return -EINVARG;
    }
    return 0;
}

/* validate the heap address alignment */
static bool heap_validate_alignment(void *ptr)
{
    return ((unsigned int)ptr % SIMPOS_HEAP_BLOCK_SIZE) == 0;
}

/* create heap and heap table */
int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table)
{
    int res = 0;

    /* if start or end addresses are not aligned */
    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end))
    {
        return -EINVARG;
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
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);
    return res;
}

/* returns four lowest bits of the entry */
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f;
}

/* finds the starting block for the memory allocation */
int heap_get_start_block(struct heap *heap, uint32_t total_blocks)
{
    struct heap_table *table = heap->table;
    int block_count = 0;
    int starting_block = -1;

    /* loop through the table entries */
    for (size_t i = 0; i < table->total; i++)
    {
        /* if entry type is not free, continue to the next entry */
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE)
        {
            block_count = 0;
            starting_block = -1;
            continue;
        }

        /* if the starting block is not set */
        if (starting_block == -1)
        {
            starting_block = i;
        }

        block_count++;

        /* if enough free blocks for the memory allocation */
        if (block_count == total_blocks)
        {
            break;
        }
    }

    /* if starting block is found */
    if (starting_block == -1)
    {
        return -ENOMEM;
    }
    return starting_block;
}

/* calculates the address of the block */
void *heap_block_to_address(struct heap *heap, int block)
{
    return heap->start_address + (block * SIMPOS_HEAP_BLOCK_SIZE);
}

/* marks blocks as taken */
void heap_mark_blocks_taken(struct heap *heap, int start_block, int total_blocks)
{
    int end_block = (start_block + total_blocks) - 1;
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;

    /* if more than one block */
    if (total_blocks > 1)
    {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    /* loop through the defined blocks */
    for (int i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;

        /* if not the last block */
        if (i != end_block - 1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

/* allocates the heap memory blocks, and returns the address of the first block */
void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks)
{
    void *address = 0;
    int start_block = heap_get_start_block(heap, total_blocks);

    if (start_block < 0)
    {
        return address;
    }

    address = heap_block_to_address(heap, start_block);
    heap_mark_blocks_taken(heap, start_block, total_blocks);
    return address;
}

/* aligns the value to the upper block size, to reserve enough space for the data */
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

/* allocate memory from heap */
void *heap_malloc(struct heap *heap, size_t size)
{
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / SIMPOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

/* calculates the block number of the memory address */
int heap_address_to_block(struct heap *heap, void *address)
{
    return ((int)(address - heap->start_address)) / SIMPOS_HEAP_BLOCK_SIZE;
}

/* mark blocks as free */
void heap_mark_blocks_free(struct heap *heap, int starting_block)
{
    struct heap_table *table = heap->table;

    /* loops from starting block to the end of the table */
    for (int i = starting_block; i < (int)table->total; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;

        /* if the entry is the last block */
        if (!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
}

/* free the allocated memory */
void heap_free(struct heap *heap, void *ptr)
{
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}