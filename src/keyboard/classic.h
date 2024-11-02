#ifndef CLASSIC_KEYBOARD_H
#define CLASSIC_KEYBOARD_H

#define PS2_COMMAND_PORT 0x64              // command register IO port
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE // enable first PS/2 port
#define PS2_DATA_PORT 0x60                 // data port
#define CLASSIC_KEYBOARD_KEY_RELEASE 0x80  // bitmask for checking if key is release
#define ISR_KEYBOARD_INTERRUPT 0x21        // index number of the keyboard interrupt

struct keyboard *classic_init();

#endif
