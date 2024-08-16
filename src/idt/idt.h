#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_desc
{
    uint16_t offset_1;     // offset bits 0 - 15
    uint16_t selector;     // segment selector
    uint8_t zero;          // does nothing, unused set to zero
    uint8_t type_attr;     // descriptor type and attributes (also used for setting bits 44-47)
    uint16_t offset_2;     // offset bits 16-31
} __attribute__((packed)); // ensures that the compiler doesn't mess up the struct with padding

struct idtr_desc
{
    uint16_t limit;        // size of descriptor table -1
    uint32_t base;         // base address of the start of the interrupt descriptor table
} __attribute__((packed)); // ensures that the compiler doesn't mess up the struct with padding

void idt_init();

#endif