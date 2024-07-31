ORG 0x7c00 ; starting address
BITS 16

message: db 'Hello World!', 0

_start:
    mov si, message
    call print
    jmp $

print:
    mov bh, 0 ; page
.loop:
    lodsb ; loads chars from si to al
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10 ; BIOS call
    ret


times 510-($-$$) db 0 ; fill the rest with 0's
dw 0xAA55 ; boot signature