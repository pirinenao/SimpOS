#ifndef KERNEL_H
#define KERNEL_H

/* macros for error value handling */
#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)(value) < 0)
void kernel_page();
void kernel_registers();

/* function prototypes */
void kernel_main();
void kernel_panic(const char *error_msg);

#endif