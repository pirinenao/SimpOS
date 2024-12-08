[BITS 32]

section .asm

global print:function
global simpos_getkey:function
global simpos_malloc:function
global simpos_free:function
global simpos_putchar:function
global simpos_process_load_start:function
global simpos_system:function
global simpos_process_get_arguments:function



; void print(const char* message)
print:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    push dword[ebp+8]   ; retrieve the function argument
    mov eax, 1          ; index of print command for interrupt 0x80
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return

; int simpos_getkey();
simpos_getkey:
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
    push dword[ebp+8]   ; retrieve the function argument
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return    

;void simpos_putchar(char c)
simpos_putchar:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, 3          ; index of putchar command for interrupt 0x80   
    push dword[ebp+8]   ; retrieve the function argument
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret    

; void simpos_free(void* ptr)
simpos_free:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, 5          ; index of free command for interrupt 0x80   
    push dword[ebp+8]   ; retrieve the function argument
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return   

; void simpos_process_load_start(const char* filename)
simpos_process_load_start:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, 6          ; index of process load start command for interrupt 0x80   
    push dword[ebp+8]   ; retrieve the function argument
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return  

; int simpos_system(struct command_argument* arguments)
simpos_system:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, 7          ; index of the system command for interrupt 0x80   
    push dword[ebp+8]   ; retrieve the function argument
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return  

; void simpos_process_get_arguments(struct process_arguments* arguments)
simpos_process_get_arguments:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, 8          ; index of process_get_arguments command for interrupt 0x80   
    push dword[ebp+8]   ; retrieve the function argument
    int 0x80            ; call the interrupt
    add esp, 4          ; restore the state of the stack
    pop ebp             ; preserve callers stack frame
    ret                 ; return  
