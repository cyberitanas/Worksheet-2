#include "../source/io.h"
#include "../source/stdint.h"
#include "framebuffer.h"

/* ==================== VGA CONSTANTS ==================== */

#define FB_ADDRESS ((uintptr_t)0x000B8000)
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT    0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15

/* ==================== INTERNAL STATE ==================== */

static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

static uint8_t fg_color = FB_WHITE;
static uint8_t bg_color = FB_BLACK;

/* A handy combined color byte */
static inline uint8_t fb_color_byte(void) {
    return (bg_color << 4) | (fg_color & 0x0F);
}

/* Pointer to VGA text memory */
static inline uint16_t* fb_ptr(uint16_t x, uint16_t y) {
    return (uint16_t*)(FB_ADDRESS + 2 * (y * FB_WIDTH + x));
}

/* ==================== CURSOR HANDLING ==================== */

void fb_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    outb(FB_COMMAND_PORT, 0x0A);
    outb(FB_DATA_PORT, (inb(FB_DATA_PORT) & 0xC0) | cursor_start);

    outb(FB_COMMAND_PORT, 0x0B);
    outb(FB_DATA_PORT, (inb(FB_DATA_PORT) & 0xE0) | cursor_end);
}

void fb_disable_cursor(void)
{
    outb(FB_COMMAND_PORT, 0x0A);
    outb(FB_DATA_PORT, 0x20);
}

static void fb_update_hw_cursor(void)
{
    uint16_t pos = cursor_y * FB_WIDTH + cursor_x;

    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos >> 8);

    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0xFF);
}

void fb_set_cursor_pos(uint16_t x, uint16_t y)
{
    if (x >= FB_WIDTH) x = FB_WIDTH - 1;
    if (y >= FB_HEIGHT) y = FB_HEIGHT - 1;

    cursor_x = x;
    cursor_y = y;
    fb_update_hw_cursor();
}

uint16_t fb_get_cursor_x(void) { return cursor_x; }
uint16_t fb_get_cursor_y(void) { return cursor_y; }

/* ==================== COLOR CONTROL ==================== */

void fb_set_color(uint8_t fg, uint8_t bg)
{
    fg_color = fg & 0x0F;
    bg_color = bg & 0x0F;
}

uint8_t fb_get_fg_color(void) { return fg_color; }
uint8_t fb_get_bg_color(void) { return bg_color; }

void fb_reset_color(void)
{
    fg_color = FB_WHITE;
    bg_color = FB_BLACK;
}

/* ==================== SCREEN CLEARING ==================== */

void fb_clear(void)
{
    uint16_t* fb = (uint16_t*)FB_ADDRESS;
    uint16_t entry = fb_color_byte() << 8 | ' ';

    for (int i = 0; i < FB_WIDTH * FB_HEIGHT; ++i)
        fb[i] = entry;

    cursor_x = 0;
    cursor_y = 0;
    fb_update_hw_cursor();
}

void fb_clear_line(uint16_t y)
{
    if (y >= FB_HEIGHT) return;

    uint16_t entry = fb_color_byte() << 8 | ' ';
    uint16_t* fb = (uint16_t*)FB_ADDRESS;

    for (int x = 0; x < FB_WIDTH; x++)
        fb[y * FB_WIDTH + x] = entry;
}

void fb_clear_area(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t entry = fb_color_byte() << 8 | ' ';

    for (uint16_t row = y; row < y + height && row < FB_HEIGHT; row++)
    {
        for (uint16_t col = x; col < x + width && col < FB_WIDTH; col++)
        {
            *fb_ptr(col, row) = entry;
        }
    }
}

/* ==================== SCROLLING ==================== */

void fb_scroll(void)
{
    uint16_t* fb = (uint16_t*)FB_ADDRESS;

    // Move lines up
    for (int i = 0; i < (FB_HEIGHT - 1) * FB_WIDTH; i++)
        fb[i] = fb[i + FB_WIDTH];

    // Clear last line
    uint16_t blank = fb_color_byte() << 8 | ' ';
    for (int i = (FB_HEIGHT - 1) * FB_WIDTH; i < FB_HEIGHT * FB_WIDTH; i++)
        fb[i] = blank;

    if (cursor_y > 0)
        cursor_y--;

    fb_update_hw_cursor();
}

/* ==================== TEXT OUTPUT ==================== */

void fb_putc(char c)
{
    if (c == '\b') {
        fb_backspace();
        return;
    }

    if (c == '\n')
    {
        fb_newline();
        return;
    }

    if (c == '\r')
    {
        cursor_x = 0;
        fb_update_hw_cursor();
        return;
    }

    *fb_ptr(cursor_x, cursor_y) = (fb_color_byte() << 8) | c;

    cursor_x++;
    if (cursor_x >= FB_WIDTH)
    {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= FB_HEIGHT)
        fb_scroll();

    fb_update_hw_cursor();
}

void fb_write(const char* str)
{
    while (*str)
        fb_putc(*str++);
}

void fb_backspace(void)
{
    if (cursor_x == 0 && cursor_y == 0) {
        return;
    }

    if (cursor_x > 0) {
        cursor_x--;
    } else {
        cursor_y--;
        cursor_x = FB_WIDTH - 1;
    }

    *fb_ptr(cursor_x, cursor_y) = (fb_color_byte() << 8) | ' ';
    fb_update_hw_cursor();
}

void fb_newline(void)
{
    cursor_x = 0;
    cursor_y++;
    if (cursor_y >= FB_HEIGHT) {
        fb_scroll();
        cursor_y = FB_HEIGHT - 1;
    }
    fb_update_hw_cursor();
}

void fb_putc_at(uint16_t x, uint16_t y, char c)
{
    if (x >= FB_WIDTH || y >= FB_HEIGHT)
        return;

    *fb_ptr(x, y) = (fb_color_byte() << 8) | c;
}

void fb_write_at(uint16_t x, uint16_t y, const char* str)
{
    fb_set_cursor_pos(x, y);
    fb_write(str);
}

/* ==================== NUMBER OUTPUT ==================== */

void fb_write_int(int num)
{
    if (num == 0) { fb_putc('0'); return; }

    if (num < 0) { fb_putc('-'); num = -num; }

    char buf[12];
    int i = 0;

    while (num > 0)
    {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i--)
        fb_putc(buf[i]);
}

void fb_write_uint(unsigned int num)
{
    if (num == 0) { fb_putc('0'); return; }

    char buf[12];
    int i = 0;

    while (num > 0)
    {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i--)
        fb_putc(buf[i]);
}

void fb_write_hex(unsigned int num)
{
    fb_write("0x");

    char hex[] = "0123456789ABCDEF";
    char buf[8];

    for (int i = 7; i >= 0; i--)
    {
        buf[i] = hex[num & 0xF];
        num >>= 4;
    }

    for (int i = 0; i < 8; i++)
        fb_putc(buf[i]);
}

void fb_write_bin(unsigned int num)
{
    fb_write("0b");
    for (int i = 31; i >= 0; i--)
        fb_putc((num & (1u << i)) ? '1' : '0');
}

/* ==================== DRAWING PRIMITIVES ==================== */

void fb_draw_hline(uint16_t x, uint16_t y, uint16_t length, char c)
{
    for (uint16_t i = 0; i < length && (x + i) < FB_WIDTH; i++)
        fb_putc_at(x + i, y, c);
}

void fb_draw_vline(uint16_t x, uint16_t y, uint16_t length, char c)
{
    for (uint16_t i = 0; i < length && (y + i) < FB_HEIGHT; i++)
        fb_putc_at(x, y + i, c);
}

void fb_draw_box(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    if (width < 2 || height < 2) return;

    fb_putc_at(x, y, '+');
    fb_draw_hline(x + 1, y, width - 2, '-');
    fb_putc_at(x + width - 1, y, '+');

    for (uint16_t i = 1; i < height - 1; i++)
    {
        fb_putc_at(x, y + i, '|');
        fb_putc_at(x + width - 1, y + i, '|');
    }

    fb_putc_at(x, y + height - 1, '+');
    fb_draw_hline(x + 1, y + height - 1, width - 2, '-');
    fb_putc_at(x + width - 1, y + height - 1, '+');
}

void fb_fill_box(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char c)
{
    for (uint16_t row = 0; row < height && (y + row) < FB_HEIGHT; row++)
        for (uint16_t col = 0; col < width && (x + col) < FB_WIDTH; col++)
            fb_putc_at(x + col, y + row, c);
}
