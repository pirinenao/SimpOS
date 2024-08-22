#include "paging.h"
#include "memory/heap/kernel_heap.h"

/* function prototype */
void paging_load_directory(uint32_t *directory);

/* variable holding current directory */
static uint32_t *current_directory = 0;

/* creates page directory, with page tables */
struct paging_4gb_chunk *paging_new_4gb(uint8_t flags)
{
    int offset = 0;

    /* allocates and nulls memory for the page directory */
    uint32_t *directory = kernel_zalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

    /* loops through the directory */
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        /* allocates and nulls memory for the page table */
        uint32_t *entry = (kernel_zalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE));

        /* calculate addresses for the pages */
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }

        /* advance to the next table */
        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    struct paging_4gb_chunk *chunk_4gb = kernel_zalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;
}

/* switches to directory */
void paging_switch(uint32_t *directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

/* returns the address of the directory */
uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk)
{
    return chunk->directory_entry;
}