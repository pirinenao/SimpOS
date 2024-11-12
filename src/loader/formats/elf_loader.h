#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "config.h"

struct elf_file
{
    char filename[SIMPOS_MAX_PATH];
    int in_memory_size;

    /* elf files physical address */
    void *elf_memory;

    void *virtual_base_address;
    void *virtual_end_address;

    void *physical_base_address;
    void *physical_end_address;
};

/* function prototypes */
int elf_load(const char *filename, struct elf_file **file_out);
void elf_close(struct elf_file *file);
void *elf_virtual_end(struct elf_file *file);
void *elf_virtual_base(struct elf_file *file);
void *elf_phys_base(struct elf_file *file);
void *elf_phys_end(struct elf_file *file);

#endif