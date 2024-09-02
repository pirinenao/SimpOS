#ifndef TERMINAL_H
#define TERMINAL_H

/* terminal constants */
#define VGA_WIDTH 80
#define VGA_HEIGHT 20
/* color constants */
#define COLOR_WHITE 15
#define COLOR_BLACK 0

/* function prototypes */
void print(const char *str);
void terminal_initialize();

#endif