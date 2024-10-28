#include "io.h"
#include "../task/task.h"
#include "../terminal/terminal.h"

void *isr80h_command1_print(struct interrupt_frame *frame)
{
    char buf[1024];

    /* receive the address of the stack item from userspace (virtual address) */
    void *user_space_msg_buffer = task_get_stack_item(task_current(), 0);

    /* copy the string from the virtual address to the buf */
    copy_string_from_task(task_current(), user_space_msg_buffer, buf, sizeof(buf));

    print(buf);
    return 0;
}
