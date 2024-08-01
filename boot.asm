ORG 0 ; set the origin to 0x0000, where BIOS will load the bootloader
BITS 16

message: db 'Hello World!', 0

_start:
    jmp short start
    nop

times 33 db 0   ; making space for BPB https://wiki.osdev.org/FAT#BPB_(BIOS_Parameter_Block)

start:
    jmp 0x7c0:step2 ; jump to the step2 label in segment 0x7C0

step2:
    cli ; clear interrupts
    ; initializing segment registers
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; enable interrupts
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

    jmp $

times 510-($-$$) db 0 ; fill the rest with 0's
dw 0xAA55 ; boot signature