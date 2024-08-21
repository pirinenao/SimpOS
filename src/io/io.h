#ifndef IO_H
#define IO_H

/* function prototypes */
unsigned char insb(unsigned short port);            // read a byte from I/O port
unsigned short insw(unsigned short port);           // read two bytes from I/O port
void outb(unsigned short port, unsigned char val);  // output a byte to I/O port
void outw(unsigned short port, unsigned short val); // output two bytes to I/O port

#endif