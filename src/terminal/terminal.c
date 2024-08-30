#include <stdint.h>
#include <stddef.h>
#include "terminal.h"
#include "string/string.h"

/* variables for terminal initialization */
uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_column = 0;

/* combines a character and its color into single 16-bit (in little-endian) */
uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

/* prints a character to the specified x, y coordinates on the screen */
void terminal_putchar(int x, int y, char c, char color)
{
    video_mem[(y * VGA_WIDTH + x)] = terminal_make_char(c, color);
}

/* prints a character to the screen while advancing the cursor position */
void terminal_writechar(char c, char color)
{
    /* if newline character */
    if (c == '\n')
    {
        terminal_column = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_column, terminal_row, c, color);
    terminal_column++;

    /* if at the last column */
    if (terminal_column >= VGA_WIDTH)
    {
        terminal_column = 0;
        terminal_row++;
    }
}

/* prints a null-terminated string to the screen */
void print(const char *str)
{
    size_t len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], COLOR_WHITE);
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
            terminal_putchar(x, y, ' ', COLOR_BLACK);
        }
    }
}