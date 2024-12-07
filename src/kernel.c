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
#include "task/tss.h"
#include "task/process.h"
#include "status.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"
#include "string/string.h"

static struct paging_4gb_chunk *kernel_chunk = 0;
struct tss tss;
struct gdt gdt_real[SIMPOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[SIMPOS_TOTAL_GDT_SEGMENTS] = {
    /* .type defines the access byte */
    {.base = 0x00, .limit = 0x00, .type = 0x00},                  /* NULL segment */
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},            /* kernel code segment */
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},            /* kernel data segment*/
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},            /* user code segment*/
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},            /* user data segment*/
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9}, /* TSS segment */

};

/* switch to the kernel page directory */
void kernel_page()
{
    kernel_registers(); // switch to the kernel registers
    paging_switch(kernel_chunk);
}

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

    /* setup TSS */
    memset(&tss, 0x00, sizeof(tss));
    /* kernel stack location */
    tss.esp0 = 0x600000;
    /* stack segment register */
    tss.ss0 = KERNEL_DATA_SELECTOR;

    /* Load TSS */
    /* 0x5 * 0x8 = 0x28 */
    tss_load(0x28);

    /* setup paging */
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    /* switch to kernel paging chunk */
    paging_switch(kernel_chunk);

    /* enable paging */
    enable_paging();

    /* register kernel commands */
    isr80h_register_commands();

    /* initialize keyboards */
    keyboard_init();

    struct process *process = 0;
    int res = process_load_switch("0:/blank.elf", &process);

    if (res != SIMPOS_ALL_OK)
    {
        kernel_panic("Failed to load blank.elf\n");
    }

    struct command_argument argument;
    strcpy(argument.argument, "testing");
    argument.next = 0x00;

    process_inject_arguments(process, &argument);
    
    task_run_first_ever_task();

    while (1)
    {
    }
}