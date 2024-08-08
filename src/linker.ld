/*
Linker script which specifies the starting address for the kernel, 
sets the output format, and defines the organization of the sections 
 in the final binary file.

The .asm section ensures that any assembly code or data is kept separate 
from C code and data, to avoid alignment issues.
*/

ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M;

    .text : ALIGN(4096)
    {
        *(.text)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }

    .asm : ALIGN(4096)
    {
        *(.asm)
    }
}