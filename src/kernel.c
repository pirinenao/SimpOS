#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kernel_heap.h"
#include "memory/paging/paging.h"

/* variables for terminal initialization */
uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_column = 0;

/* combines a character and its color into single 16-bit (in little-endian) */
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

static struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_main()
{
    terminal_initialize();
    print("Hello World\n");

    /*initialize heap*/
    kernel_heap_init();

    /*initialize interrupt description table*/
    idt_init();

    /* setup paging */
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    /* switch to kernel paging chunk */
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    /* enable paging */
    enable_paging();

    /*
     *   testing the paging functionality
     */

    /* allocates a block of memory */
    char *ptr1 = kernel_zalloc(4096);
    /* set virtual address 0x1000 to point to that block */
    paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void *)0x1000, (uint32_t)ptr1 | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE | PAGING_IS_PRESENT);

    /* pointer to the new virtual address 0x1000, and adding a character to that address */
    char *ptr2 = (char *)0x1000;
    *ptr2 = 'A';
    /* prints ptr1, which prints out the character we just set to the virtual address 0x1000 */
    print(ptr1);

    /* enable interrupts */
    enable_interrupts();
}