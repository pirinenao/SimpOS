#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "../io/io.h"

struct idt_desc idt_descriptors[SIMPOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

/* external assembly functions */
extern void idt_load(struct idtr_desc *ptr);
extern void int21h(void);
extern void no_interrupt(void);

void int21h_handler()
{
    print("keyboard pressed\n");
    outb(0x20, 0x20); // acknowledge the interrupt
}

void no_interrupt_handler()
{
    outb(0x20, 0x20); // acknowledge the interrupt
}

/* test function for interrupt zero */
void idt_zero(void)
{
    print("Divide by zero error\n");
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
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1; // sets the IDT limit
    idtr_descriptor.base = (uint32_t)idt_descriptors;    // sets the base address

    for (int i = 0; i < SIMPOS_TOTAL_INTERRUPTS; i++) // initialize every entry in the IDT with default handler
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, idt_zero); // create test interrupt
    idt_set(0x21, int21h);

    idt_load(&idtr_descriptor); // loads the interrupt description table
}