[BITS 32]

section .asm

global _start

_start:

    push message    ; push message to stack
    mov eax, 1      ; Command
    int 0x80        ; call interrupt 0x80
    add esp, 4      ; adjust the stack pointer
    jmp $           ; infinite jump

section .data
message: db 'Hello from the user program!', 0