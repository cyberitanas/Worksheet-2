#include "../source/io.h"
#include "../source/stdint.h"
#include "framebuffer.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

static char key_buffer = 0;

/* US QWERTY Scancode -> ASCII */
static const char scancode_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',  0,
    '\\','z','x','c','v','b','n','m',',','.','/',  0,
    '*',  0, ' ', 0,
};

/* IRQ1 Handler */
void keyboard_handler(void)
{
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    /* Ignore key releases */
    if (scancode & 0x80)
        return;

    key_buffer = scancode_map[scancode];

    if (key_buffer)
        fb_putc(key_buffer);
}

/* Non-blocking read */
char keyboard_getchar(void)
{
    char c = key_buffer;
    key_buffer = 0;
    return c;
}
