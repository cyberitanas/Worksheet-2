/* ============================================
   framebuffer.h - Complete 2D Framebuffer API
   ============================================ */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/* Color constants */
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
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN   14
#define FB_WHITE         15

/* Screen dimensions */
#define FB_WIDTH  80
#define FB_HEIGHT 25

/* ==================== Cursor Management ==================== */

/** fb_set_cursor_pos:
 * Sets the cursor position to the specified (x, y) coordinates
 * @param x Column position (0-79)
 * @param y Row position (0-24)
 */
void fb_set_cursor_pos(unsigned int x, unsigned int y);

/** fb_get_cursor_x:
 * Returns the current cursor X position
 * @return Current column (0-79)
 */
unsigned int fb_get_cursor_x(void);

/** fb_get_cursor_y:
 * Returns the current cursor Y position
 * @return Current row (0-24)
 */
unsigned int fb_get_cursor_y(void);

/** fb_enable_cursor:
 * Enables the hardware cursor with specified appearance
 * @param cursor_start Starting scanline (0-15)
 * @param cursor_end Ending scanline (0-15)
 */
void fb_enable_cursor(unsigned char cursor_start, unsigned char cursor_end);

/** fb_disable_cursor:
 * Disables the hardware cursor
 */
void fb_disable_cursor(void);
/** fb_move:
 * Moves the cursor to the specified (x, y) coordinates
 * @param x Column position (0-79)
 * @param y Row position (0-24)
 */
void fb_move(unsigned short x, unsigned short y);


/* ==================== Text Output ==================== */

/** fb_putc:
 * Writes a single character at the current cursor position
 * @param c Character to write
 */
void fb_putc(char c);

/** fb_write:
 * Writes a null-terminated string at the current cursor position
 * @param str String to write
 */
void fb_write(const char* str);

/** fb_write_at:
 * Writes a string at the specified position
 * @param x Column position (0-79)
 * @param y Row position (0-24)
 * @param str String to write
 */
void fb_write_at(unsigned int x, unsigned int y, const char* str);

/** fb_putc_at:
 * Writes a single character at the specified position
 * @param x Column position (0-79)
 * @param y Row position (0-24)
 * @param c Character to write
 */
void fb_putc_at(unsigned int x, unsigned int y, char c);

/* ==================== Number Formatting ==================== */

/** fb_write_int:
 * Writes a signed integer at the current cursor position
 * @param num Integer to write
 */
void fb_write_int(int num);

/** fb_write_uint:
 * Writes an unsigned integer at the current cursor position
 * @param num Unsigned integer to write
 */
void fb_write_uint(unsigned int num);

/** fb_write_hex:
 * Writes a hexadecimal number at the current cursor position
 * @param num Number to write in hex format (0xXXXXXXXX)
 */
void fb_write_hex(unsigned int num);

/** fb_write_bin:
 * Writes a binary number at the current cursor position
 * @param num Number to write in binary format
 */
void fb_write_bin(unsigned int num);

/* ==================== Color Management ==================== */

/** fb_set_color:
 * Sets the current text color (foreground and background)
 * @param fg Foreground color (0-15)
 * @param bg Background color (0-15)
 */
void fb_set_color(unsigned char fg, unsigned char bg);

/** fb_get_fg_color:
 * Gets the current foreground color
 * @return Foreground color (0-15)
 */
unsigned char fb_get_fg_color(void);

/** fb_get_bg_color:
 * Gets the current background color
 * @return Background color (0-15)
 */
unsigned char fb_get_bg_color(void);

/** fb_reset_color:
 * Resets color to default (white on black)
 */
void fb_reset_color(void);

/* ==================== Screen Management ==================== */

/** fb_clear:
 * Clears the entire screen with the current background color
 */
void fb_clear(void);

/** fb_clear_line:
 * Clears a specific line with the current background color
 * @param y Row to clear (0-24)
 */
void fb_clear_line(unsigned int y);

/** fb_clear_area:
 * Clears a rectangular area
 * @param x Starting column
 * @param y Starting row
 * @param width Width of area
 * @param height Height of area
 */
void fb_clear_area(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

/** fb_scroll:
 * Scrolls the screen up by one line
 */
void fb_scroll(void);

/* ==================== Drawing Primitives ==================== */

/** fb_draw_hline:
 * Draws a horizontal line
 * @param x Starting column
 * @param y Row
 * @param length Length of line
 * @param c Character to use for line
 */
void fb_draw_hline(unsigned int x, unsigned int y, unsigned int length, char c);

/** fb_draw_vline:
 * Draws a vertical line
 * @param x Column
 * @param y Starting row
 * @param length Length of line
 * @param c Character to use for line
 */
void fb_draw_vline(unsigned int x, unsigned int y, unsigned int length, char c);

/** fb_draw_box:
 * Draws a box outline
 * @param x Starting column
 * @param y Starting row
 * @param width Width of box
 * @param height Height of box
 */
void fb_draw_box(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

/** fb_fill_box:
 * Draws a filled box
 * @param x Starting column
 * @param y Starting row
 * @param width Width of box
 * @param height Height of box
 * @param c Character to fill with
 */
void fb_fill_box(unsigned int x, unsigned int y, unsigned int width, unsigned int height, char c);

#endif /* FRAMEBUFFER_H */