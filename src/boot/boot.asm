ORG 0x7c00
BITS 16

_start:
    jmp short start ; jump over the disk format information
    nop

times 33 db 0   ; making space for BPB https://wiki.osdev.org/FAT#BPB_(BIOS_Parameter_Block)

start:
    jmp 0:step2 ; jump to the step2 label in segment 0x7C0

step2:
    cli ; clear interrupts
    ; initializing segment registers
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; enable interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]

    ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    ; jmp CODE_SEG:load32 ; far jump ( jmp to other segment)
    jmp $


; Global description table 
; https://wiki.osdev.org/Global_Descriptor_Table
; https://www.youtube.com/watch?v=Wh5nPn2U_1w

gdt_start:
gdt_null:   ; empty descriptor
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0      ; Base first 0-15 bits
    db 0      ; Base 16-23 bits
    db 10011010b   ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0      ; Base 24-31 bits

gdt_data:     ; DS, SS, ES, FS, GS
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0      ; Base first 0-15 bits
    db 0      ; Base 16-23 bits
    db 10010010b   ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0      ; Base 24-31 bits

gdt_end: 

gdt_descriptor:
    dw gdt_end - gdt_start-1    ; size of the gdt
    dd gdt_start                ; pointer to gdt

times 510-($-$$) db 0 ; fill the rest with 0's
dw 0xAA55 ; boot signature