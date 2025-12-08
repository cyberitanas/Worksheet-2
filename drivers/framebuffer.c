
#include "../source/io.h"
#include "../source/stdint.h"



// Framebuffer constants
#define FB_ADDRESS ((uintptr_t)0x000B8000)
#define FB_WIDTH 80
#define FB_HEIGHT 25

// I/O ports for cursor
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

// Commands
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

// Color codes
#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_BROWN 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

// Current position and color
static unsigned short cursor_x = 0;
static unsigned short cursor_y = 0;
static unsigned char current_color = FB_WHITE | (FB_BLACK << 4);

// Get framebuffer memory location for (x, y)
static unsigned char* fb_get_position(unsigned short x, unsigned short y)
{
    return (unsigned char*)(FB_ADDRESS + 2 * (y * FB_WIDTH + x));
}

// Move hardware cursor
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}

// Write character at current position
void fb_write_cell(unsigned char c, unsigned char fg, unsigned char bg)
{
    unsigned char* fb = fb_get_position(cursor_x, cursor_y);
    fb[0] = c;
    fb[1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

// Set cursor position
void fb_move(unsigned short x, unsigned short y)
{
    if (x >= FB_WIDTH) x = FB_WIDTH - 1;
    if (y >= FB_HEIGHT) y = FB_HEIGHT - 1;
    
    cursor_x = x;
    cursor_y = y;
    
    fb_move_cursor(y * FB_WIDTH + x);
}

// Scroll screen up one line
static void fb_scroll(void)
{
    unsigned char* fb = (unsigned char*)FB_ADDRESS;
    
    // Copy each line to the one above
    for (int i = 0; i < (FB_HEIGHT - 1) * FB_WIDTH; i++) {
        fb[i * 2] = fb[(i + FB_WIDTH) * 2];
        fb[i * 2 + 1] = fb[(i + FB_WIDTH) * 2 + 1];
    }
    
    // Clear last line
    for (int i = 0; i < FB_WIDTH; i++) {
        fb[((FB_HEIGHT - 1) * FB_WIDTH + i) * 2] = ' ';
        fb[((FB_HEIGHT - 1) * FB_WIDTH + i) * 2 + 1] = current_color;
    }
    
    cursor_y = FB_HEIGHT - 1;
}

// Advance cursor (with newline and scroll handling)
static void fb_advance_cursor(void)
{
    cursor_x++;
    
    if (cursor_x >= FB_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        
        if (cursor_y >= FB_HEIGHT) {
            fb_scroll();
        }
    }
    
    fb_move_cursor(cursor_y * FB_WIDTH + cursor_x);
}

// Write a single character
void fb_putc(char c)
{
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= FB_HEIGHT) {
            fb_scroll();
        }
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
        if (cursor_x >= FB_WIDTH) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= FB_HEIGHT) {
                fb_scroll();
            }
        }
    } else {
        fb_write_cell(c, current_color & 0x0F, (current_color >> 4) & 0x0F);
        fb_advance_cursor();
    }
}

// Write a string
void fb_write(const char* str)
{
    while (*str) {
        fb_putc(*str++);
    }
}

// Set text color
void fb_set_color(unsigned char fg, unsigned char bg)
{
    current_color = (bg << 4) | (fg & 0x0F);
}

// Clear screen
void fb_clear(void)
{
    unsigned char* fb = (unsigned char*)FB_ADDRESS;
    
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT; i++) {
        fb[i * 2] = ' ';
        fb[i * 2 + 1] = current_color;
    }
    
    cursor_x = 0;
    cursor_y = 0;
    fb_move_cursor(0);
}

// Write integer
void fb_write_int(int num)
{
    if (num == 0) {
        fb_putc('0');
        return;
    }
    
    if (num < 0) {
        fb_putc('-');
        num = -num;
    }
    
    char buffer[12];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        fb_putc(buffer[--i]);
    }
}

// Write hex number
void fb_write_hex(unsigned int num)
{
    fb_write("0x");
    
    char hex[] = "0123456789ABCDEF";
    char buffer[9];
    
    for (int i = 7; i >= 0; i--) {
        buffer[i] = hex[num & 0xF];
        num >>= 4;
    }
    buffer[8] = '\0';
    
    fb_write(buffer);
}
