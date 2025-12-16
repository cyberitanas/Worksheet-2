#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#include "../include/types.h"

#define KEYBOARD_MAX_ASCII 0x53

u8int keyboard_read_scan_code(void);
u8int keyboard_scan_code_to_ascii(u8int scan_code);

#endif
