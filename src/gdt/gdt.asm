section .asm
global gdt_load

gdt_load:
    mov eax, [esp+4]                ; retrieve the first argument
    mov [gdt_descriptor + 2], eax   ; set the start address
    mov ax, [esp+8]                 ; retrive the second argument
    mov [gdt_descriptor], ax        ; set the size
    lgdt [gdt_descriptor]           ; load gdt to gdtr
    ret                             ; return

section .data
gdt_descriptor:
    dw 0x00 ; size
    dd 0x00 ; gdt start address
