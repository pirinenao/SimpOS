[BITS 32]

global print:function
global getkey:function
global simpos_malloc:function


; void print(const char* message)
print:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    push dword[ebp+8]   ; retrieve the function argmument
    mov eax, 1          ; index of print command for interrupt 0x80
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return

; int getkey();
getkey:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, 2          ; index of getkey command for interrupt 0x80   
    int 0x80            ; call the interrupt
    pop ebp             ; preserve callers stack frame
    ret                 ; return

; void* simpos_malloc(size_t size)
simpos_malloc:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, 4          ; index of malloc command for interrupt 0x80   
    push dword[ebp+8]   ; retrieve the function argmument
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return    