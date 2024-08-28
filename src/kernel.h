#ifndef KERNEL_H
#define KERNEL_H

/* terminal constants */
#define VGA_WIDTH 80
#define VGA_HEIGHT 20
/* color constants */
#define COLOR_WHITE 15
#define COLOR_BLACK 0
/* path constants */
#define SIMPOS_MAX_PATH 108

/* function prototypes */
void kernel_main();
void print(const char *str);

#endif