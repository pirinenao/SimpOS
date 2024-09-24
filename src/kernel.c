#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "terminal/terminal.h"
#include "memory/heap/kernel_heap.h"
#include "disk/disk.h"
#include "idt/idt.h"
#include "memory/paging/paging.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"

static struct paging_4gb_chunk *kernel_chunk = 0;
struct gdt gdt_real[SIMPOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[SIMPOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},       /* NULL segment */
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a}, /* kernel code segment */
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92}  /* kernel data segment*/

};

/* prints the error message and goes into infinite loop to prevent damage */
void kernel_panic(const char *error_msg)
{
    print(error_msg);
    while (1)
    {
    }
}

void kernel_main()
{
    /* initialize terminal */
    terminal_initialize();

    /* print to the screen */
    print("Hello World\n");

    /* initalize gdt */
    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, SIMPOS_TOTAL_GDT_SEGMENTS);

    /* loads the gdt to gdtr */
    gdt_load(gdt_real, sizeof(gdt_real));

    /*initialize heap*/
    kernel_heap_init();

    /* initialize filesystem */
    fs_init();

    /* initialize the disks */
    disk_init();

    /*initialize interrupt description table*/
    idt_init();

    /* setup paging */
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    /* switch to kernel paging chunk */
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    /* enable paging */
    enable_paging();

    /* enable interrupts */
    enable_interrupts();
}