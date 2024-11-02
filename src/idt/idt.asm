section .asm

; external C functions
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

; global functions
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

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

no_interrupt:
    pushad                      ; push general purpose registers to the stack
    call no_interrupt_handler   ; call the C function
    popad                       ; pop general purpose registers from the stack
    iret                        ; interrupt return

; macro for creating interrupts
%macro interrupt 1
    global int%1
    int%1:
        pushad                  ; pushes the general purpose registers
        push esp                ; pushes the stack pointer
        push dword %1           ; push the interrupt number to the stack for interrupt_handler
        call interrupt_handler  ; call the interrupt_handler function
        add esp, 8              ; adjust the stack
        popad                   ; pop general purpose registers
        iret                    ; interrupt return
%endmacro

; macro which calls interrupt macro n times
%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

isr80h_wrapper:
    pushad                  ; pushes the general purpose registers
    push esp                ; pushes the stack pointer
    push eax                ; push the command from eax to stack for the c handler function
    call isr80h_handler     ; call the c handler function
    mov dword[tmp_res], eax ; move the handler functions return value to tmp_res
    add esp, 8              ; adjust the stack
    popad                   ; pop general purpose registers
    mov eax, [tmp_res]
    iretd

section .data
; stores the return result from isr80h_handler
tmp_res: dd 0

; macros for creating the interrupt pointer table
%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep

