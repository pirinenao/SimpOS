#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kernel_heap.h"

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_column = 0;

/* combines a character and its color into single 16-bit value in little-endian format */
uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

/* counts string length */
size_t strlen(const char *str)
{
    size_t length = 0;
    while (str[length] != '\0')
    {
        length++;
    }

    return length;
}

/* prints a char to the specified x, y coordinates on the screen */
void terminal_putchar(int x, int y, char c, char color)
{
    video_mem[(y * VGA_WIDTH + x)] = terminal_make_char(c, color);
}

/* prints a character to the screen while advancing the cursor position */
void terminal_writechar(char c, char color)
{
    if (c == '\n')
    {
        terminal_column = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_column, terminal_row, c, color);
    terminal_column++;

    if (terminal_column >= VGA_WIDTH)
    {
        terminal_column = 0;
        terminal_row++;
    }
}

/* prints a null-terminated string to the screen, character by character */
void print(const char *str)
{
    size_t len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

/* clears the screen */
void terminal_initialize()
{
    video_mem = (uint16_t *)(0xB8000);
    terminal_row = 0;
    terminal_column = 0;

    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

void kernel_main()
{
    terminal_initialize();
    print("Hello World\n");

    /*initialize heap*/
    kernel_heap_init();

    /*initialize interrupt description table*/
    idt_init();

    /* test calls for the kernel_malloc */
    void *ptr = kernel_malloc(50);
    void *ptr2 = kernel_malloc(5000);
}