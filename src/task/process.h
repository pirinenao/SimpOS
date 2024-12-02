#ifndef PROCESS_H
#define PROCESS_H
#include "config.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>

#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

typedef unsigned char PROCESS_FILETYPE;

struct process_allocation
{
    void *ptr;
    size_t size;
};

/* describes the process */
struct process
{
    uint16_t id;
    char filename[SIMPOS_MAX_PATH];
    struct task *task;
    /* keeps track of allocations to avoid memory leaks */
    struct process_allocation allocations[SIMPOS_MAX_PROGRAM_ALLOCATIONS];

    PROCESS_FILETYPE filetype;

    union
    {
        /* pointer to the process memory */
        void *ptr;
        struct elf_file *elf_file;
    };

    /* pointer to the process stack memory */
    void *stack;
    /* size of the data pointer by ptr */
    uint32_t size;
    /* keyboard buffer */
    struct keyboard_buffer
    {
        char buffer[SIMPOS_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

/* function prototypes */
int process_load_switch(const char *filename, struct process **process);
int process_switch(struct process *process);
int process_load_for_slot(const char *filename, struct process **process, int process_slot);
int process_load(const char *filename, struct process **process);
struct process *process_current();
void *process_malloc(struct process *process, size_t size);
void process_free(struct process *process, void *ptr);

#endif