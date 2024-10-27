#include "misc.h"
#include "../idt/idt.h"
#include "../task/task.h"

/* only for test purposes */
void *isr80h_command0_sum(struct interrupt_frame *frame)
{
    int value1 = (int)task_get_stack_item(task_current(), 0);
    int value2 = (int)task_get_stack_item(task_current(), 1);
    return (void *)value1 + value2;
}