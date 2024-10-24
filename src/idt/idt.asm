section .asm

; external C functions
extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler

; global functions
global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push ebp                ; preserve callers stack frame
    mov ebp, esp            ; establish a new stack frame
    mov ebx, [ebp+8]        ; access the first argument
    lidt [ebx]              ; load IDT (interrupt descriptor table) to CPU's IDTR (interrupt descriptor table register)
    pop ebp                 ; restore previous stack frame
    ret

int21h:

    pushad                  ; push general purpose registers to the stack
    call int21h_handler     ; call the C function
    popad                   ; pop general purpose registers from the stack
    iret                    ; interrupt return

no_interrupt:
    pushad                      ; push general purpose registers to the stack
    call no_interrupt_handler   ; call the C function
    popad                       ; pop general purpose registers from the stack
    iret                        ; interrupt return

isr80h_wrapper:
    pushad                  ; pushes the general purpose registers
    push esp                ; pushes the stack pointer
    push eax                ; push the command from eax to stack for the c handler function
    call isr80h_handler     ; call the c handler function
    mov dword[tmp_res], eax ; move the handler functions return value to tmp_res
    add esp, 8              ; adjuts the stack
    popad                   ; pop general purpose registers
    mov eax, [tmp_res]
    iretd

section .data
; stores the return result from isr80h_handler
tmp_res: dd 0
