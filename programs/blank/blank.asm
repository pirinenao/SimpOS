[BITS 32]

section .asm

global _start

_start:
    push 30
    push 20
    mov eax, 0  ; index of the desired command 
    int 0x80    ; call the interrupt 0x80
    add esp, 8  ; restore stack

    jmp $