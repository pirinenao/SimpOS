section .asm

extern int21h_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt

idt_load:
    push ebp                ; save the old base pointer
    mov ebp, esp            ; set up the new base pointer
    mov ebx, [ebp+8]        ; access the first argument
    lidt [ebx]              ; load instruction descriptor table
    pop ebp                 ; restore the old base pointer
    ret

int21h:
    cli                     ; stop interrupts
    pushad                  ; push general purpose registers to the stack
    call int21h_handler     ; call the C function
    popad                   ; pop general purpose registers from the stack
    sti                     ; start interrupts
    iret                    ; interrupt return

no_interrupt:
    cli                         ; stop interrupts
    pushad                      ; push general purpose registers to the stack
    call no_interrupt_handler   ; call the C function
    popad                       ; pop general purpose registers from the stack
    sti                         ; start interrupts
    iret                        ; interrupt return