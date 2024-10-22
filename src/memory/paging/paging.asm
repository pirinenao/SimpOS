[BITS 32]

section .asm

; global functions
global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, [ebp+8]    ; access the first argument
    mov cr3, eax        ; load address of the page directory
    pop ebp             ; restore previous stack frame
    ret

enable_paging:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, cr0        ; take the value from cr0
    or eax, 0x80000000  ; set the last bit up to enable paging
    mov cr0, eax        ; move the modified value to cr0
    pop ebp             ; restore previous stack frame
    ret
