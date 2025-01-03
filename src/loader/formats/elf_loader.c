#include "memory/memory.h"
#include "memory/paging/paging.h"
#include "memory/heap/kernel_heap.h"
#include "string/string.h"
#include "elf_loader.h"
#include "fs/file.h"
#include "kernel.h"
#include "config.h"
#include "status.h"
#include "fs/file.h"
#include "elf.h"
#include <stdbool.h>

const char elf_signature[] = {0x7f, 'E', 'L', 'F'};

/* checks if signature matches to the elf signature */
static bool elf_valid_signature(void *buffer)
{
    return memcmp(buffer, (void *)elf_signature, sizeof(elf_signature)) == 0;
}

/* checks if compatible for 32-bit architecture */
static bool elf_valid_class(struct elf_header *header)
{
    return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

/* checks if encoded into little-endian */
static bool elf_valid_encoding(struct elf_header *header)
{
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

/* checks if the file is executable and the entry is in safe address */
static bool elf_is_executable(struct elf_header *header)
{
    return header->e_type == ET_EXEC && header->e_entry >= SIMPOS_PROGRAM_VIRTUAL_ADDRESS;
}

/* checks if has program header */
static bool elf_has_program_header(struct elf_header *header)
{
    return header->e_phoff != 0;
}

/* returns pointer to the elf files physical address */
void *elf_memory(struct elf_file *file)
{
    return file->elf_memory;
}

/* returns the elf header */
struct elf_header *elf_header(struct elf_file *file)
{
    return file->elf_memory;
}

/* returns the section header */
struct elf32_shdr *elf_sheader(struct elf_header *header)
{
    return (struct elf32_shdr *)((int)header + header->e_shoff);
}

/* check if file has program header and return it */
struct elf32_phdr *elf_pheader(struct elf_header *header)
{
    if (header->e_phoff == 0)
    {
        return 0;
    }

    return (struct elf32_phdr *)((int)header + header->e_phoff);
}

/* returns a program header entry based on the given index */
struct elf32_phdr *elf_program_header(struct elf_header *header, int index)
{
    return &elf_pheader(header)[index];
}

/* returns a section header entry based on the given index */
struct elf32_shdr *elf_section(struct elf_header *header, int index)
{
    return &elf_sheader(header)[index];
}

/* returns the string table*/
char *elf_str_table(struct elf_header *header)
{
    return (char *)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

/* returns the virtual base address */
void *elf_virtual_base(struct elf_file *file)
{
    return file->virtual_base_address;
}

/* returns the virtual end address */
void *elf_virtual_end(struct elf_file *file)
{
    return file->virtual_end_address;
}

/* returns the physical base address */
void *elf_phys_base(struct elf_file *file)
{
    return file->physical_base_address;
}

/* returns the physical end address */
void *elf_phys_end(struct elf_file *file)
{
    return file->physical_end_address;
}

/* returns the physical program header address */
void *elf_phdr_phys_address(struct elf_file *file, struct elf32_phdr *phdr)
{
    return elf_memory(file) + phdr->p_offset;
}

/* validate the elf file */
int elf_validate_loaded(struct elf_header *header)
{
    return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) && elf_is_executable(header) && elf_has_program_header(header)) ? SIMPOS_ALL_OK : -EINFORMAT;
}

/* calculates base and end addresses for the program header */
int elf_process_phdr_pt_load(struct elf_file *elf_file, struct elf32_phdr *phdr)
{
    /* base addresses */
    if (elf_file->virtual_base_address >= (void *)phdr->p_vaddr || elf_file->virtual_base_address == 0x00)
    {
        elf_file->virtual_base_address = (void *)phdr->p_vaddr;
        elf_file->physical_base_address = elf_memory(elf_file) + phdr->p_offset;
    }

    /* end addresses */
    unsigned int end_virtual_address = phdr->p_vaddr + phdr->p_filesz;
    if (elf_file->virtual_end_address <= (void *)(end_virtual_address) || elf_file->virtual_end_address == 0x00)
    {
        elf_file->virtual_end_address = (void *)end_virtual_address;
        elf_file->physical_end_address = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }
    return 0;
}

/* processes program header */
int elf_process_pheader(struct elf_file *elf_file, struct elf32_phdr *phdr)
{
    int res = 0;
    switch (phdr->p_type)
    {
    case PT_LOAD:
        res = elf_process_phdr_pt_load(elf_file, phdr);
        break;
    }

    return res;
}

/* processes all the available program headers */
int elf_process_pheaders(struct elf_file *elf_file)
{
    int res = 0;
    struct elf_header *header = elf_header(elf_file);

    /* loops through all the program headers */
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct elf32_phdr *phdr = elf_program_header(header, i);
        res = elf_process_pheader(elf_file, phdr);
        if (res < 0)
        {
            break;
        }
    }

    return res;
}

/* validates the file and processes the program headers  */
int elf_process_loaded(struct elf_file *elf_file)
{
    int res = 0;
    struct elf_header *header = elf_header(elf_file);
    res = elf_validate_loaded(header);
    if (res < 0)
    {
        return res;
    }

    res = elf_process_pheaders(elf_file);
    if (res < 0)
    {
        return res;
    }

    return res;
}

/* free the elf file */
void elf_file_free(struct elf_file* elf_file)
{
    if(elf_file->elf_memory)
    {
        kfree(elf_file->elf_memory);
    }

    kfree(elf_file);

}

/* allocate memory for elf file */
struct elf_file* elf_file_new()
{
    return (struct elf_file*) kzalloc(sizeof(struct elf_file));
}

/* loads and initializes the elf file */
int elf_load(const char *filename, struct elf_file **file_out)
{
    struct elf_file *elf_file = elf_file_new();
    int fd = 0;
    int res = fopen(filename, "r");
    if (res <= 0)
    {
        return -EIO;
        goto out;
    }
    fd = res;

    /* fetch file information */
    struct file_stat stat;
    res = fstat(fd, &stat);

    if (res < 0)
    {
        goto out;
    }

    /* allocate memory for the file */
    elf_file->elf_memory = kzalloc(stat.file_size);
    /* read the file into the allocated memory */
    res = fread(elf_file->elf_memory, stat.file_size, 1, fd);

    if (res < 0)
    {
        goto out;
    }

    /* process the loaded elf file */
    res = elf_process_loaded(elf_file);

    if (res < 0)
    {
        goto out;
    }
    /* point to the memory address of our elf file */
    *file_out = elf_file;
out:
    if(res < 0)
    {
        elf_file_free(elf_file);
    }
    fclose(fd);
    return res;
}

/* close the elf file by releasing the allocated memory */
void elf_close(struct elf_file *file)
{
    if (!file)
    {
        return;
    }

    kfree(file->elf_memory);
    kfree(file);
}