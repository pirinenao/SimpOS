#ifndef CONFIG_H
#define CONFIG_H

/* system constants */
#define KERNEL_CODE_SELECTOR 0x08                // selector for kernel code segment
#define KERNEL_DATA_SELECTOR 0x10                // selector for kernel data segment
#define SIMPOS_TOTAL_INTERRUPTS 512              // total number of interrupts supported
#define SIMPOS_HEAP_SIZE_BYTES 104857600         // size of the heap in bytes (100 MB)
#define SIMPOS_HEAP_BLOCK_SIZE 4096              // size of each heap block in bytes (4 KB)
#define SIMPOS_HEAP_ADDRESS 0x01000000           // starting address of the heap
#define SIMPOS_HEAP_TABLE_ADDRESS 0x00007E00     // address of the heap table
#define SIMPOS_SECTOR_SIZE 512                   // hard disk sector size
#define SIMPOS_MAX_PATH 108                      // max path length
#define SIMPOS_MAX_FILESYSTEMS 12                // max number of filesystems
#define SIMPOS_MAX_FILE_DESCRIPTORS 512          // max number of file descriptors (open files)
#define SIMPOS_TOTAL_GDT_SEGMENTS 6              // max number of gdt segments
#define SIMPOS_PROGRAM_VIRTUAL_ADDRESS 0x400000  // virtual address for programs
#define SIMPOS_USER_PROGRAM_STACK_SIZE 1024 * 16 // userspace stack size
#define SIMPOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define SIMPOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END (SIMPOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START - SIMPOS_USER_PROGRAM_STACK_SIZE)
#define SIMPOS_MAX_PROGRAM_ALLOCATIONS 1024
#define SIMPOS_MAX_PROCESSES 12
#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b
#define SIMPOS_MAX_ISR80H_COMMANDS 1024
#endif