#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* bitmasks for setting up page flags */
#define PAGING_CACHE_DISABLED 0b00010000
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITEABLE 0b00000010
#define PAGING_IS_PRESENT 0b00000001

/* constants for paging */
#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

/* structure for paging */
struct paging_4gb_chunk
{
    uint32_t *directory_entry; // address to the directory
};

/* function prototypes */
void enable_paging();
void paging_switch(uint32_t *directory);
struct paging_4gb_chunk *paging_new_4gb(uint8_t flags);
uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk);
int paging_set(uint32_t *directory, void *virtual_address, uint32_t physical_address);
bool validate_paging_alignment(void *address);
void paging_free_4gb(struct paging_4gb_chunk *chunk);
int paging_map_to(uint32_t *directory, void *virt, void *phys, void *phys_end, int flags);
int paging_map_range(uint32_t *directory, void *virt, void *phys, int count, int flags);
int paging_map(uint32_t *directory, void *virt, void *phys, int flags);
void *paging_align_address(void *ptr);

#endif