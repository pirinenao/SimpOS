ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short start     ; jump over the disk format information
    nop

times 33 db 0           ; making space for BPB https://wiki.osdev.org/FAT#BPB_(BIOS_Parameter_Block)

start:
    jmp 0:step2         ; jump to the step2 label in segment 0x7C0

step2:
    cli                 ; clear interrupts
    ; initializing segment registers
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti                 ; enable interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]

    ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32  ; far jump ( jmp to other segment)

; Global description table 
; https://wiki.osdev.org/Global_Descriptor_Table
; https://www.youtube.com/watch?v=Wh5nPn2U_1w
gdt_start:
gdt_null:               ; empty descriptor
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xffff           ; Segment limit first 0-15 bits
    dw 0                ; Base first 0-15 bits
    db 0                ; Base 16-23 bits
    db 10011010b        ; Access byte
    db 11001111b        ; High 4 bit flags and the low 4 bit flags
    db 0                ; Base 24-31 bits

gdt_data:               ; DS, SS, ES, FS, GS
    dw 0xffff           ; Segment limit first 0-15 bits
    dw 0                ; Base first 0-15 bits
    db 0                ; Base 16-23 bits
    db 10010010b        ; Access byte
    db 11001111b        ; High 4 bit flags and the low 4 bit flags
    db 0                ; Base 24-31 bits

gdt_end: 

gdt_descriptor:
    dw gdt_end - gdt_start-1    ; size of the gdt
    dd gdt_start                ; pointer to gdt

; loading kernel to the memory
[BITS 32]
load32:
    mov eax, 1              ; starting sector
    mov ecx, 100            ; total number of sectors we want to load
    mov edi, 0x0100000      ; the address we want to load the sectors into
    call ata_lba_read
    jmp CODE_SEG:0x0100000

; reading sectors from hard disk using ATA interface
; https://wiki.osdev.org/ATA_read/write_sectors
ata_lba_read:
    mov ebx, eax            ; backup the lba

    ; send the highest 8 bits of the lba to hard disk controller
    shr eax, 24             ; shift bits
    or eax, 0xE0            ; select the master drive
    mov dx, 0x1F6           ; define port
    out dx, al              ; send data to port
    
    ; send the amount of total sectors to read
    mov eax, ecx
    mov dx, 0x1F2           ; define port
    out dx, al              ; send data to port

    ; send the starting sector
    mov eax, ebx
    mov dx, 0x1F3           ; define port
    out dx, al              ; send data to port
        
    ; send bits
    mov eax, ebx
    mov dx, 0x1F4           ; define port
    shr eax, 8              ; shift bits
    out dx, al              ; send data to port

    ; send upper 16 bits
    mov eax, ebx 
    mov dx, 0x1F5           ; define port
    shr eax, 16             ; shift bits
    out dx, al              ; send data to port

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

    ; loop for reading the sectors into the memory
    
.next_sector:
    push ecx                ; save the loop counter (number of sectors) into to stack

    ; wait for the disk to be ready
.try_again:
    mov dx, 0x1F7           ; port 0x1F7: status register
    in al, dx               ; Read the status from the port
    test al, 8              ; check if the data request bit is set
    jz .try_again           ; try again if the bit is not set

    ; read a sector (512 bytes)
    mov ecx, 256            ; define how many times insw repeats
    mov dx, 0x1F0
    rep insw                ; input word from port specified in dx into memory specified in di

    pop ecx
    loop .next_sector       ; decrement ecx and jump to .next_sector if ecx is not zero

    ret

times 510-($-$$) db 0       ; fill the rest with 0's
dw 0xAA55                   ; boot signature