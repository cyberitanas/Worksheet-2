#pragma once

/* --- Constants --- */
// Keyboard I/O Port Address
#define KEYBOARD_DATA_PORT 0x60

/* --- External Declarations --- */
// I/O Port Access function
extern unsigned char inb(unsigned short port);

// Keyboard data map and buffer
extern const unsigned char scancode_map[];
extern char key_buffer;

/* --- Keyboard API --- */
char keyboard_getchar(void);
void keyboard_handler(void);

/* --- PIC API --- */
// This part is already correct, but ensure pic.c is linked
void pic_acknowledge(unsigned int irq);