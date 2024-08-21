section .asm

; external C functions
extern int21h_handler
extern no_interrupt_handler

; global functions
global int21h
global idt_load
global no_interrupt

idt_load:
    push ebp                ; preserve callers stack frame
    mov ebp, esp            ; establish a new stack frame
    mov ebx, [ebp+8]        ; access the first argument
    lidt [ebx]              ; load IDT (interrupt descriptor table) to CPU's IDTR (interrupt descriptor table register)
    pop ebp                 ; restore previous stack frame
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