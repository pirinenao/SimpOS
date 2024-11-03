[BITS 32]

section .asm

global _start

_start:
    call getKey
    push message
    mov eax, 1
    int 0x80
    add esp, 4

    jmp $           ; infinite jump

getKey:
    mov eax, 2      ; command getkey
    int 0x80        ; call the interrupt 0x80
    cmp eax, 0x00   ; compare interrupts return value to zero
    je getKey       ; if equal to zero, jump back to the label
    ret             ; return

section .data
message: db 'Hello from the user program!', 0