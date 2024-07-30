ORG 0x7c00 ; starting address
BITS 16

_start:
    mov ah, 0eh
    mov al, 'X'
    mov bh, 0
    int 0x10 ; BIOS call

    jmp $

times 510-($-$$) db 0 ; fill the rest with 0's
dw 0xAA55 ; boot signature