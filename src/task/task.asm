[BITS 32]

section .asm

global restore_general_purpose_registers
global task_return
global user_registers

restore_general_purpose_registers:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    add esp, 4          ; restore previous stack frame
    ret                 ; return

task_return:
    mov ebp, esp        ; establish a new stack frame
    mov ebx, [ebp+4]    ; access the struct
    push dword [ebx+44] ; push the data/stack selector
    push dword [ebx+40] ; push the stack pointer
    pushf               ; push the flags
    pop eax
    or eax, 0x200
    push eax
    push dword [ebx+32] ; push the code segment
    push dword [ebx+28] ; push the ip
    
    ; setup segment registers
    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebp+4]
    call restore_general_purpose_registers

    add esp, 4          ; restore stack frame
    iretd               ; return to the address of ip

user_registers:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret
