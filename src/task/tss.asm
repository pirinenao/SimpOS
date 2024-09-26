section .asm

global tss_load

tss_load:
    push ebp        ; preserve callers stack frame
    mov ebp, esp    ; establish a new stack frame
    mov ax, [ebp+8] ; TSS Segment
    ltr ax          ; load task register
    pop ebp         ; restore previous stack frame
    ret             ; return