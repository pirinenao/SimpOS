#ifndef GDT_H
#define GDT_H
#include <stdint.h>

/* holds the gdt entry byte array */
struct gdt
{
    uint16_t segment;        /* Segment limit first 0-15 bits */
    uint16_t base_first;     /* Base first 0-15 bits */
    uint8_t base;            /* Base 16-23 bits */
    uint8_t access;          /* Access byte */
    uint8_t high_flags;      /* High 4 bit flags and the low 4 bit limit */
    uint8_t base_24_31_bits; /* Base 24-31 bits */
} __attribute__((packed));

/* structured version of gdt entry */
/* gets turned into byte array before loaded to gdtr */
struct gdt_structured
{
    uint32_t base;  /* base address (32-bit) */
    uint32_t limit; /* limit */
    uint8_t type;   /* access rights/type */
};

/* function prototypes */

void gdt_load(struct gdt *gdt, int size);
void gdt_structured_to_gdt(struct gdt *gdt, struct gdt_structured *structured_gdt, int total_entries);
#endif