#include "memory/heap/kernel_heap.h"
#include "memory/paging/paging.h"
#include "memory/memory.h"
#include "loader/formats/elf_loader.h"
#include "fs/file.h"
#include "string/string.h"
#include "kernel.h"
#include "status.h"
#include "process.h"
#include "config.h"
#include "task.h"

struct process *current_process = 0;
static struct process *processes[SIMPOS_MAX_PROCESSES] = {};

static void process_init(struct process *process)
{
    memset(process, 0, sizeof(struct process));
}

struct process *process_current()
{
    return current_process;
}

/* returns process based on the process_id */
struct process *process_get(int process_id)
{
    /* ensures that the process id is in boundaries */
    if (process_id < 0 || process_id >= SIMPOS_MAX_PROCESSES)
    {
        return NULL;
    }

    return processes[process_id];
}

/* finds free slot from the process->allocations array */
static int process_find_free_allocation_index(struct process *process)
{
    int res = -ENOMEM;
    for (int i = 0; i < SIMPOS_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == 0)
        {
            res = 1;
            break;
        }
    }
    return res;
}

/* check if the pointer can be found from the process allocations list */
static bool process_is_process_pointer(struct process *process, void *ptr)
{
    for (int i = 0; i < SIMPOS_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == ptr)
        {
            return true;
        }
    }

    return false;
}

/* removes the allocation from the process allocations list */
static void remove_process_allocation(struct process *process, void *ptr)
{

    for (int i = 0; i < SIMPOS_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == ptr)
        {
            process->allocations[i].ptr = 0x00;
            process->allocations[i].size = 0;
        }
    }
}

/* returns the allocation address */
static struct process_allocation *process_get_allocation_addr(struct process *process, void *addr)
{
    for (int i = 0; i < SIMPOS_MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == addr)
        {
            return &process->allocations[i];
        }
    }

    return 0;
}

/* free process allocated memory */
void process_free(struct process *process, void *ptr)
{
    struct process_allocation *allocation = process_get_allocation_addr(process, ptr);

    /* returns if the pointer doesn't belong to this process */
    if (!allocation)
    {
        return;
    }

    /* removes the flags from the allocation so its not accessible*/
    int res = paging_map_to(process->task->page_directory, allocation->ptr, allocation->ptr, paging_align_address(allocation->ptr + allocation->size), 0x00);

    if (res < 0)
    {
        return;
    }
    remove_process_allocation(process, ptr);
    kfree(ptr);
}

/* abstracted memory allocation function for stdlib to avoid memory leaks */
void *process_malloc(struct process *process, size_t size)
{
    void *ptr = kzalloc(size);
    if (!ptr)
    {
        goto out_error;
    }

    int index = process_find_free_allocation_index(process);
    if (index < 0)
    {
        goto out_error;
    }

    int res = paging_map_to(process->task->page_directory, ptr, ptr, paging_align_address(ptr + size), PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (res < 0)
    {
        goto out_error;
    }

    process->allocations[index].ptr = ptr;
    process->allocations[index].size = size;

    return ptr;

out_error:
    if (ptr)
    {
        kfree(ptr);
    }

    return 0;
}

/* switch current process to the given process */
int process_switch(struct process *process)
{
    current_process = process;
    return 0;
}

/* loads the binary file into memory */
static int process_load_binary(const char *filename, struct process *process)
{
    int res = 0;

    int fd = fopen(filename, "r");
    if (!fd)
    {
        return -EIO;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res != SIMPOS_ALL_OK)
    {
        return res;
    }

    void *program_data_ptr = kzalloc(stat.file_size);
    if (!program_data_ptr)
    {
        return -ENOMEM;
    }

    if (fread(program_data_ptr, stat.file_size, 1, fd) != 1)
    {
        return -EIO;
    }

    process->filetype = PROCESS_FILETYPE_BINARY;
    process->ptr = program_data_ptr;
    process->size = stat.file_size;

    fclose(fd);
    return res;
}

static int process_load_elf(const char *filename, struct process *process)
{
    int res = 0;
    struct elf_file *elf_file = 0;
    res = elf_load(filename, &elf_file);
    if (res < 0)
    {
        return res;
    }

    process->filetype = PROCESS_FILETYPE_ELF;
    process->elf_file = elf_file;
    return res;
}

static int process_load_data(const char *filename, struct process *process)
{
    int res = 0;

    res = process_load_elf(filename, process);

    /* if the file is not ELF format, load it as binary */
    if (res == -EINFORMAT)
    {
        res = process_load_binary(filename, process);
    }
    return res;
}

int process_map_binary(struct process *process)
{
    int res = 0;
    res = paging_map_to(process->task->page_directory, (void *)SIMPOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

static int process_map_elf(struct process *process)
{
    int res = 0;
    struct elf_file *elf_file = process->elf_file;
    struct elf_header *header = elf_header(elf_file);
    struct elf32_phdr *phdrs = elf_pheader(header);
    /* loop through all the program headers */
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct elf32_phdr *phdr = &phdrs[i];
        void *phdr_phys_address = elf_phdr_phys_address(elf_file, phdr);
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (phdr->p_flags & PF_W)
        {
            flags |= PAGING_IS_WRITEABLE;
        }

        res = paging_map_to(process->task->page_directory, paging_align_to_lower_page((void *)phdr->p_vaddr), paging_align_to_lower_page(phdr_phys_address), paging_align_address(phdr_phys_address + phdr->p_memsz), flags);
        if (res < 0)
        {
            return res;
        }
    }
    return res;
}

int process_map_memory(struct process *process)
{
    int res = 0;

    switch (process->filetype)
    {
    case PROCESS_FILETYPE_ELF:
        res = process_map_elf(process);
        break;

    case PROCESS_FILETYPE_BINARY:
        res = process_map_binary(process);
        break;

    default:
        kernel_panic("process_map_memory: Invalid filetype\n");
    }

    if (res < 0)
    {
        return res;
    }

    /* map the stack */
    paging_map_to(process->task->page_directory, (void *)SIMPOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack + SIMPOS_USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

/* finds a free slot from the processes array */
int process_get_free_slot()
{
    for (int i = 0; i < SIMPOS_MAX_PROCESSES; i++)
    {
        if (processes[i] == 0)
        {
            return i;
        }
    }

    return -EISTKN;
}

/* loads process and switches to it */
int process_load_switch(const char *filename, struct process **process)
{
    int res = process_load(filename, process);
    if (res == 0)
    {
        process_switch(*process);
    }

    return res;
}

/* find a free slot for the process and loads to the process there */
int process_load(const char *filename, struct process **process)
{
    int process_slot = process_get_free_slot();
    if (process_slot < 0)
    {
        return -EISTKN;
    }

    return process_load_for_slot(filename, process, process_slot);
}

/* load process for the given slot */
int process_load_for_slot(const char *filename, struct process **process, int process_slot)
{
    int res = 0;
    struct task *task = 0;
    struct process *_process;
    void *program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        res = EISTKN;
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    /* allocate stack memory for the program */
    program_stack_ptr = kzalloc(SIMPOS_USER_PROGRAM_STACK_SIZE);
    if (!program_stack_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));

    _process->stack = program_stack_ptr;
    _process->id = process_slot;

    /* create a task */
    task = task_new(_process);

    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
        goto out;
    }

    _process->task = task;

    res = process_map_memory(_process);
    if (res < 0)
    {
        goto out;
    }

    *process = _process;

    /* add the process to the array */
    process[process_slot] = _process;
out:
    if (ISERR(res))
    {
        if (_process && _process->task)
        {
            task_free(_process->task);
        }

        /* FREE THE PROCESS DATA */
    }

    return res;
}
