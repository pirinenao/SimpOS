#ifndef IDT_H
#define IDT_H
#include <stdint.h>

struct interrupt_frame;
typedef void *(*ISR80_COMMAND)(struct interrupt_frame *frame);
typedef void (*INTERRUPT_CALLBACK_FUNCTION)();

/* structure for interrupt description table entries */
struct idt_desc
{
    uint16_t offset_1;     // offset bits 0 - 15
    uint16_t selector;     // segment selector
    uint8_t zero;          // does nothing, unused set to zero
    uint8_t type_attr;     // descriptor type and attributes (also used for setting bits 44-47)
    uint16_t offset_2;     // offset bits 16-31
} __attribute__((packed)); // ensures that the compiler doesn't mess up the structure with padding

/* structure for interrupt description table register */
struct idtr_desc
{
    uint16_t limit;        // size of descriptor table -1
    uint32_t base;         // starting address of the interrupt descriptor table
} __attribute__((packed)); // ensures that the compiler doesn't mess up the structure with padding

/* general purpose registers for interrupts */
struct interrupt_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;

} __attribute__((packed)); // ensures that the compiler doesn't mess up the structure with padding

/* function prototypes */
void idt_init();
void enable_interrupts();
void disable_interrupts();
void isr80h_register_command(int command_id, ISR80_COMMAND command);
int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback);

#endif