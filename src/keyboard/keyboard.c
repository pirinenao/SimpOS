#include "task/process.h"
#include "task/task.h"
#include "keyboard.h"
#include "status.h"
#include "kernel.h"
#include "classic.h"

static struct keyboard *keyboard_list_head = 0;
static struct keyboard *keyboard_list_last = 0;

/* initialize keyboards */
void keyboard_init()
{
    keyboard_insert(classic_init());
}

/* adds a new keyboard to the system */
int keyboard_insert(struct keyboard *keyboard)
{
    /* if keyboard doesn't have initialization function */
    if (!keyboard->init)
    {
        return -EINVARG;
    }

    if (keyboard_list_last)
    {
        keyboard_list_last->next = keyboard;
        keyboard_list_last = keyboard;
    }
    else
    {
        keyboard_list_head = keyboard;
        keyboard_list_last = keyboard;
    }

    return keyboard->init();
}

/* gets the tail index of buffer (uses remainder to avoid overflows) */
static int keyboard_get_tail_index(struct process *process)
{
    return process->keyboard.tail % sizeof(process->keyboard.buffer);
}

/* backspace functionality */
void keyboard_backspace(struct process *process)
{
    process->keyboard.tail -= 1;
    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = 0x00;
}

/* pushes character to current processes keyboard buffer */
void keyboard_push(char c)
{
    struct process *process = process_current();
    if (!process)
    {
        return;
    }

    if (c == 0)
    {
        return;
    }

    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = c;
    process->keyboard.tail++;
}

/* pop a character from current tasks keyboard buffer */
char keyboard_pop()
{
    if (!task_current())
    {
        return 0;
    }

    struct process *process = task_current()->process;
    int real_index = process->keyboard.head % sizeof(process->keyboard.buffer);
    char c = process->keyboard.buffer[real_index];

    /* if nothing to pop */
    if (c == 0x00)
    {
        return 0;
    }

    process->keyboard.buffer[real_index] = 0;
    process->keyboard.head++;
    return c;
}