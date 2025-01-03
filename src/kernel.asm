[BITS 32]

; global functions
global _start
global kernel_registers

; external C functions
extern kernel_main

; constants for segment offsets
CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    ;sets up the data segment registers and the stack pointer
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; remap master PIC (programmable interupt controller)
    mov al, 00010001b   ; init mode
    out 0x20, al        ; send to command port
    mov al, 0x20        ; master ISR starting address
    out 0x21, al        ; send to data port
    mov al, 0x04
    out 0x21, al        ; map the slave 
    mov al, 00000001b   ; x86 mode
    out 0x21, al        ; send to data port

    call kernel_main    ; call the kernel_main C function

    jmp $               ; infinite jump     


kernel_registers:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    ret


times 512-($-$$) db 0   ; fill the rest of the sector with 0's