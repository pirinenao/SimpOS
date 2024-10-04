#include "process.h"
#include "../config.h"
#include "memory/memory.h"
#include "../status.h"
#include "task.h"
#include "../fs/file.h"
#include "../string/string.h"
#include "../memory/heap/kernel_heap.h"
#include "../kernel.h"
#include "../memory/paging/paging.h"

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

int process_get(int process_id)
{
    /* ensures that the process id is in boundaries */
    if (process_id < 0 || process_id >= SIMPOS_MAX_PROCESSES)
    {
        return -EINVARG;
    }

    return processes[process_id];
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

    void *program_data_ptr = kczalloc(stat.filesize);
    if (!program_data_ptr)
    {
        return -ENOMEM;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1)
    {
        return -EIO;
    }

    process->ptr = program_data_ptr;
    process->size = stat.filesize;

    fclose(fd);
    return res;
}

static int process_load_data(const char *filename, struct process *process)
{
    int res = 0;
    res = process_load_binary(filename, process);
    return res;
}

int process_map_binary(struct process *process)
{
    int res = 0;
    paging_map_to(process->task->page_directory->directory_entry, (void *)SIMPOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
}

int process_map_memory(struct process *process)
{
    int res = 0;
    res = process_map_binary(process);
    return res;
}

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

    strcpy(_process->filename, filename, sizeof(_process->filename));

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

    res = process_map_memory(process);
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
        if (_process && process->task)
        {
            task_free(_process->task);
        }

        /* FREE THE PROCESS DATA */
    }

    return res;
}