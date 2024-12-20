[BITS 32]

global _start
extern c_start
extern simpos_exit

section .asm

_start:
    call c_start
    call simpos_exit
    ret
