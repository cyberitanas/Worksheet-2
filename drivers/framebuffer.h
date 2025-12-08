/* ============================================
   framebuffer.h - VGA Text Mode Framebuffer API
   ============================================ */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../source/stdint.h"

/* ==================== Color Constants ==================== */

#define FB_BLACK         0
#define FB_BLUE          1
#define FB_GREEN         2
#define FB_CYAN          3
#define FB_RED           4
#define FB_MAGENTA       5
#define FB_BROWN         6
#define FB_LIGHT_GREY    7
#define FB_DARK_GREY     8
#define FB_LIGHT_BLUE    9
#define FB_LIGHT_GREEN   10
#define FB_LIGHT_CYAN    11
#define FB_LIGHT_RED     12
#define FB_PINK          13
#define FB_YELLOW        14
#define FB_WHITE         15

/* Screen dimensions */
#define FB_WIDTH   80
#define FB_HEIGHT  25

/* ==================== Cursor Control ==================== */

/** fb_enable_cursor:
 * Enables the VGA hardware cursor.
 */
void fb_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

/** fb_set_cursor_pos:
 * Sets cursor position based on X,Y.
 */
void fb_set_cursor_pos(uint16_t x, uint16_t y);

/** fb_get_cursor_x:
 * Gets current cursor X.
 */
uint16_t fb_get_cursor_x(void);

/** fb_get_cursor_y:
 * Gets current cursor Y.
 */
uint16_t fb_get_cursor_y(void);

/* ==================== Text Output ==================== */

void fb_putc(char c);
void fb_write(const char* str);

/** fb_putc_at:
 * Writes one character at (x,y) without moving global cursor.
 */
void fb_putc_at(uint16_t x, uint16_t y, char c);

/** fb_write_at:
 * Writes a string at (x,y).
 */
void fb_write_at(uint16_t x, uint16_t y, const char* str);

/* ==================== Number Output ==================== */

void fb_write_int(int num);
void fb_write_uint(unsigned int num);
void fb_write_hex(unsigned int num);
void fb_write_bin(unsigned int num);

/* ==================== Color Management ==================== */

void fb_set_color(uint8_t fg, uint8_t bg);
uint8_t fb_get_fg_color(void);
uint8_t fb_get_bg_color(void);
void fb_reset_color(void);

/* ==================== Screen Management ==================== */

void fb_clear(void);
void fb_clear_line(uint16_t y);
void fb_clear_area(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void fb_scroll(void);

/* ==================== Drawing Primitives ==================== */

void fb_draw_hline(uint16_t x, uint16_t y, uint16_t length, char c);
void fb_draw_vline(uint16_t x, uint16_t y, uint16_t length, char c);
void fb_draw_box(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void fb_fill_box(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char c);

#endif /* FRAMEBUFFER_H */
