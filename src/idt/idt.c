#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"

struct idt_desc idt_descriptors[SIMPOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc *ptr);

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
    idt_load(&idtr_descriptor);                          // loads the interrupt description table (by using external assembly function defined in idt.asm)

    idt_set(0, idt_zero); // create test interrupt
}