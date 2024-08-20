[BITS 32]
global _start
extern kernel_main
CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; enabling A20 line for full memory access
    ; https://wiki.osdev.org/A20_Line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; remap master PIC (programmable interupt controller)
    mov al, 00010001b   ; init mode
    out 0x20, al        ; send to command port
    mov al, 0x20        ; master ISR starting address
    out 0x21, al        ; send to data port
    mov al, 00000001b   ; x86 mode
    out 0x21, al        ; send to data port


    call kernel_main

    jmp $

times 512-($-$$) db 0           ; align to 16 bytes to avoid alignment issues with C objects