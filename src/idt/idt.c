#include "idt.h"
#include "config.h"
#include "memory/memory.h"
#include "../io/io.h"
#include "../terminal/terminal.h"

/* defined structures */
struct idt_desc idt_descriptors[SIMPOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

/* external assembly functions */
extern void idt_load(struct idtr_desc *ptr);
extern void int21h(void);
extern void no_interrupt(void);

/* interrupt 21h handler: handles the keyboard interrupt */
void int21h_handler()
{
    print("keyboard pressed\n");
    outb(0x20, 0x20); // acknowledge the interrupt
}

/* default handler which doesn't have any functionality */
void no_interrupt_handler()
{
    outb(0x20, 0x20); // acknowledge the interrupt
}

/* function for setting up interrupt service routines */
void idt_set(int interrupt_no, void *address)
{
    struct idt_desc *desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t)address & 0x0000ffff; // zero out the upper bits
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t)address >> 16; // shift the upper 16 bits into the lower 16 bits
}

/* initializes IDT and IDTR  */
void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors)); // sets all the IDT entries to zero
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1; // sets the IDT size
    idtr_descriptor.base = (uint32_t)idt_descriptors;    // sets the starting address of the IDT

    /* initialize every IDT entry with default handler */
    for (int i = 0; i < SIMPOS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    /* create entry for interrupt 21h */
    idt_set(0x21, int21h);

    /* loads the IDT into IDTR */
    idt_load(&idtr_descriptor);
}