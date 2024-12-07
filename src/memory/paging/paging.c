#include "memory/heap/kernel_heap.h"
#include "paging.h"
#include "status.h"

/* function prototype */
void paging_load_directory(uint32_t *directory);

/* variable holding current directory */
static uint32_t *current_directory = 0;

/* creates page directory, with page tables */
struct paging_4gb_chunk *paging_new_4gb(uint8_t flags)
{
    int offset = 0;

    /* allocates and nulls memory for the page directory */
    uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

    /* loops through the directory */
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        /* allocates and nulls memory for the page table */
        uint32_t *entry = (kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE));

        /* calculate addresses for the pages */
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }

        /* advance to the next table */
        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    struct paging_4gb_chunk *chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;
}

/* switches to directory */
void paging_switch(struct paging_4gb_chunk *directory)
{
    paging_load_directory(directory->directory_entry);
    current_directory = directory->directory_entry;
}

void paging_free_4gb(struct paging_4gb_chunk *chunk)
{
    /* loops throught the page tables */
    for (int i = 0; i < 1024; i++)
    {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t *table = (uint32_t *)(entry & 0xfffff000);
        kfree(table);
    }

    kfree(chunk->directory_entry);
    kfree(chunk);
}

/* returns the address of the directory */
uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk)
{
    return chunk->directory_entry;
}

/* returns true if address is valid for paging */
bool validate_paging_alignment(void *address)
{
    return ((uint32_t)address % PAGING_PAGE_SIZE) == 0;
}

/* calculates which directory and table entries are responsible for the virtual address */
int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
    if (!validate_paging_alignment(virtual_address))
    {
        return -EINVARG;
    }

    /* calculate the directory index */
    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    /* calculate the page table index */
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

    return 0;
}

/* aligns to the page size */
void *paging_align_address(void *ptr)
{
    if ((uint32_t)ptr % PAGING_PAGE_SIZE)
    {
        return (void *)(uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE);
    }

    return ptr;
}

/* align to the lower page */
void *paging_align_to_lower_page(void *addr)
{
    uint32_t _addr = (uint32_t)addr;
    _addr -= (_addr % PAGING_PAGE_SIZE);
    return (void *)_addr;
}

/* maps a page */
int paging_map(struct paging_4gb_chunk *directory, void *virt, void *phys, int flags)
{
    /* if not aligned with the page size */
    if (((unsigned int)virt % PAGING_PAGE_SIZE) || ((unsigned int)phys % PAGING_PAGE_SIZE))
    {
        return -EINVARG;
    }

    return paging_set(directory->directory_entry, virt, (uint32_t)phys | flags);
}

/* maps given amount of pages */
int paging_map_range(struct paging_4gb_chunk *directory, void *virt, void *phys, int count, int flags)
{
    int res = 0;
    for (int i = 0; i < count; i++)
    {
        res = paging_map(directory, virt, phys, flags);
        if (res < 0)
        {
            break;
        }

        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }

    return res;
}

int paging_map_to(struct paging_4gb_chunk *directory, void *virt, void *phys, void *phys_end, int flags)
{
    int res = 0;
    if ((uint32_t)virt % PAGING_PAGE_SIZE)
    {
        return -EINVARG;
    }

    if ((uint32_t)phys % PAGING_PAGE_SIZE)
    {
        return -EINVARG;
    }

    if ((uint32_t)phys_end % PAGING_PAGE_SIZE)
    {
        return -EINVARG;
    }

    if ((uint32_t)phys_end < (uint32_t)phys)
    {
        return -EINVARG;
    }

    uint32_t total_bytes = phys_end - phys;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    res = paging_map_range(directory, virt, phys, total_pages, flags);

    return res;
}

/* returns a physical address of the given virtual address */
uint32_t paging_get(uint32_t *directory, void *virt)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_indexes(virt, &directory_index, &table_index);
    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    return table[table_index];
}

/* makes virtual address to point a physical address */
int paging_set(uint32_t *directory, void *virtual_address, uint32_t physical_address)
{
    if (!validate_paging_alignment(virtual_address))
    {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;

    int res = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if (res < 0)
    {
        return res;
    }

    uint32_t entry = directory[directory_index];
    /* pointer to the extracted page table address */
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    /* sets the address */
    table[table_index] = physical_address;

    return res;
}

void* paging_get_physical_address(uint32_t* directory, void* virt)
{
    void* virt_addr_new = (void*) paging_align_to_lower_page(virt);
    void* difference = (void*)((uint32_t) virt - (uint32_t) virt_addr_new);
    return (void*)((paging_get(directory, virt_addr_new) & 0xfffff000) + difference);
}