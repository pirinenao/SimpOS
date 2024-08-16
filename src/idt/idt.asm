section .asm

global idt_load
idt_load:
    push ebp                ; save the old base pointer
    mov ebp, esp            ; set up the new base pointer
    mov ebx, [ebp+8]        ; access the first argument
    lidt [ebx]              ; load instruction descriptor table
    pop ebp                 ; restore the old base pointer
    ret                     