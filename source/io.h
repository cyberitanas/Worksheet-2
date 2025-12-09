#ifndef IO_H
#define IO_H

// Write an 8-bit value to an I/O port
void outb(unsigned short port, unsigned char data);

// Write a 16-bit value to an I/O port (used for QEMU shutdown)
void outw(unsigned short port, unsigned short value);

// Read an 8-bit value from an I/O port
unsigned char inb(unsigned short port);

#endif
