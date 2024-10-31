#ifndef CLASSIC_KEYBOARD_H
#define CLASSIC_KEYBOARD_H

#define PS2_PORT 0x64                       // command register IO port
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE  // enable first PS/2 port

struct keyboard *classic_init();

#endif
