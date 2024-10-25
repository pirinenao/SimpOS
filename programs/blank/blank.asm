[BITS 32]

section .asm

global _start

_start:
    mov eax, 0  ; set the desired command index to eax register
    int 0x80    ; call the interrupt 0x80

    jmp $