section .asm

; global functions
global insb
global insw
global outb
global outw

insb:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    xor eax, eax        ; zero out the eax register
    mov edx, [ebp+8]    ; move the first argument (port address) into edx
    in al, dx           ; read a byte from I/O port defined in dx, and store it in the al
    pop ebp             ; restore previous stack frame
    ret                 ; return

insw:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    xor eax, eax        ; zero out the eax register
    mov edx, [ebp+8]    ; move the first argument (port address) into edx
    in ax, dx           ; read two bytes from I/O port defined in dx, and store it in the al
    pop ebp             ; restore previous stack frame
    ret                 ; return

outb:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, [ebp+12]   ; move the second argument (value to output) into eax
    mov edx, [ebp+8]    ; move the first argument (port address) into edx
    out dx, al          ; output byte from al to I/O port defined in dx
    pop ebp             ; restore previous stack frame
    ret                 ; return

outw:
    push ebp            ; preserve callers stack frame
    mov ebp, esp        ; establish a new stack frame
    mov eax, [ebp+12]   ; move the second argument (value to output) into eax
    mov edx, [ebp+8]    ; move the first argument (port address) into edx
    out dx, ax          ; output two bytes from ax to I/O port defined in dx
    pop ebp             ; restore previous stack frame
    ret                 ; return