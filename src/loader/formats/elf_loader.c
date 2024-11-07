#include "elf_loader.h"
#include "../../fs/file.h"
#include <stdbool.h>
#include "../../memory/memory.h"
#include "../../memory/heap/kernel_heap.h"
#include "../../string/string.h"
#include "../../memory/paging/paging.h"
#include "../../kernel.h"
#include "../../config.h"

const char *elf_signature[] = {0x7f, 'E', 'L', 'F'};

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
static bool elf_valid_encoding(struct elf_ehader *header)
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
    return file->elf_memory
}

/* returns the elf header */
struct elf_header *elf_header(struct elf_file *file)
{
    return file->elf_memory;
}

/* returns the section header */
struct elf32_shrd *elf_sheader(struct elf_header *header)
{
    return (struct elf32_shr*)(int)header+header->e_shoff);
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
struct elf32_phdr *elf_has_program_header(struct elf_header *header, int index)
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

/* validate the elf file */
int elf_validate_loaded(struct elf_header *header)
{
    return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) && elf_has_program_header(header)) ? SIMPOS_ALL_OK : -EINVARG;
}
