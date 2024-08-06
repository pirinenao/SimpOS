/*
Linker script which specifies the starting address for the kernel, 
sets the output format, and defines the organization of the sections 
 in the final binary file.
*/

ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M;

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}