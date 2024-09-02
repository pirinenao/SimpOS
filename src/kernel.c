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

static struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_main()
{
    /* initialize terminal */
    terminal_initialize();

    /* print to the screen */
    print("Hello World\n");

    /*initialize heap*/
    kernel_heap_init();

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