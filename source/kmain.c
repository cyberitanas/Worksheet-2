#include "../drivers/framebuffer.h"
#include "../drivers/keyboard.h"
#include "../source/idt.h"
#include "../source/pic.h"

/* ===================== Helpers ===================== */

#define INPUT_BUF_SIZE 64

/* Simple test function from before */
int sum_of_three(int a, int b, int c) {
    return a + b + c;
}

/* Blocking read of a single character from keyboard */
static char read_char_blocking(void)
{
    char c = 0;
    while ((c = keyboard_getchar()) == 0) {
        /* spin until we get a char */
    }
    return c;
}

/* Line input with basic echo + backspace.
 * Writes at current cursor position, stores into buf, null-terminated.
 * Returns length (not including null terminator).
 */
static int read_line(char *buf, int max_len)
{
    int len = 0;

    while (1) {
        char c = read_char_blocking();

        if (c == '\r') {
            /* treat CR as newline */
            c = '\n';
        }

        if (c == '\n') {
            fb_putc('\n');
            break;
        } else if (c == '\b' || c == 127) {
            /* backspace */
            if (len > 0) {
                len--;
                /* move cursor back, overwrite with space, move back again */
                uint16_t x = fb_get_cursor_x();
                uint16_t y = fb_get_cursor_y();
                if (x > 0) {
                    x--;
                } else if (y > 0) {
                    y--;
                    x = FB_WIDTH - 1;
                }
                fb_set_cursor_pos(x, y);
                fb_putc(' ');
                fb_set_cursor_pos(x, y);
            }
        } else if (c >= 32 && c <= 126) {
            /* printable ASCII */
            if (len < max_len - 1) {
                buf[len++] = c;
                fb_putc(c);
            }
        }
    }

    buf[len] = '\0';
    return len;
}

/* Very simple decimal parser: supports optional leading '-' */
static int parse_int(const char *s, int *out_ok)
{
    int neg = 0;
    int value = 0;
    int i = 0;
    *out_ok = 0;

    if (s[0] == '-') {
        neg = 1;
        i = 1;
    }

    if (s[i] == '\0') {
        return 0;
    }

    for (; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') {
            return 0;
        }
        value = value * 10 + (s[i] - '0');
    }

    if (neg) value = -value;
    *out_ok = 1;
    return value;
}

/* ============== Demo / Examples Section ============== */

static void show_examples(void)
{
    fb_clear();
    fb_set_color(FB_CYAN, FB_BLACK);
    fb_draw_box(0, 0, FB_WIDTH, 3);
    fb_write_at(2, 1, "Welcome to Tiny OS!");
    fb_set_cursor_pos(0, 4);

    /* Color tests */
    fb_set_color(FB_GREEN, FB_BLACK);
    fb_write("This is green text\n");

    fb_set_color(FB_RED, FB_BLACK);
    fb_write("This is red text\n");

    fb_set_color(FB_CYAN, FB_BLUE);
    fb_write("Cyan on blue!\n\n");

    fb_reset_color();

    /* Number tests */
    fb_write("Testing numbers: ");
    fb_write_int(12345);
    fb_write("\n");

    fb_write("Hex value: ");
    fb_write_hex(0xDEADBEEF);
    fb_write("\n\n");

    /* Math test */
    fb_write("Math tests:\n");
    fb_write("5 + 3 + 2 = ");
    fb_write_int(sum_of_three(5, 3, 2));
    fb_write("\n\n");
}

/* ============== Math Menu ============== */

static void math_menu(void)
{
    char buf[INPUT_BUF_SIZE];

    while (1) {
        fb_set_color(FB_YELLOW, FB_BLACK);
        fb_write("=== Math Menu ===\n");
        fb_reset_color();
        fb_write("1) Sum of three numbers\n");
        fb_write("2) Back to main menu\n");
        fb_write("Select option: ");

        read_line(buf, INPUT_BUF_SIZE);

        if (buf[0] == '2') {
            fb_write("\nReturning to main menu...\n\n");
            return;
        } else if (buf[0] == '1') {
            int ok1, ok2, ok3;
            int a, b, c;

            fb_write("Enter first number: ");
            read_line(buf, INPUT_BUF_SIZE);
            a = parse_int(buf, &ok1);

            fb_write("Enter second number: ");
            read_line(buf, INPUT_BUF_SIZE);
            b = parse_int(buf, &ok2);

            fb_write("Enter third number: ");
            read_line(buf, INPUT_BUF_SIZE);
            c = parse_int(buf, &ok3);

            if (!ok1 || !ok2 || !ok3) {
                fb_set_color(FB_RED, FB_BLACK);
                fb_write("Invalid input. Please enter integers only.\n\n");
                fb_reset_color();
            } else {
                int result = sum_of_three(a, b, c);
                fb_set_color(FB_GREEN, FB_BLACK);
                fb_write("Result: ");
                fb_write_int(result);
                fb_write("\n\n");
                fb_reset_color();
            }
        } else {
            fb_set_color(FB_RED, FB_BLACK);
            fb_write("Unknown option. Try again.\n\n");
            fb_reset_color();
        }
    }
}

/* ============== Color Menu ============== */

static void color_menu(void)
{
    char buf[INPUT_BUF_SIZE];

    while (1) {
        fb_set_color(FB_CYAN, FB_BLACK);
        fb_write("=== Color Menu ===\n");
        fb_reset_color();
        fb_write("1) White on Black (default)\n");
        fb_write("2) Green on Black (matrix vibe)\n");
        fb_write("3) Cyan on Blue (retro)\n");
        fb_write("4) Yellow on Black\n");
        fb_write("5) Back to main menu\n");
        fb_write("Select option: ");

        read_line(buf, INPUT_BUF_SIZE);

        if (buf[0] == '5') {
            fb_write("\nReturning to main menu...\n\n");
            return;
        } else if (buf[0] == '1') {
            fb_set_color(FB_WHITE, FB_BLACK);
            fb_write("\nColor set to White on Black.\n\n");
        } else if (buf[0] == '2') {
            fb_set_color(FB_GREEN, FB_BLACK);
            fb_write("\nColor set to Green on Black.\n\n");
        } else if (buf[0] == '3') {
            fb_set_color(FB_CYAN, FB_BLUE);
            fb_write("\nColor set to Cyan on Blue.\n\n");
        } else if (buf[0] == '4') {
            fb_set_color(FB_YELLOW, FB_BLACK);
            fb_write("\nColor set to Yellow on Black.\n\n");
        } else {
            fb_set_color(FB_RED, FB_BLACK);
            fb_write("Unknown option. Try again.\n\n");
            fb_reset_color();
        }
    }
}

/* ============== Main Menu ============== */

static void show_main_menu(void)
{
    fb_set_color(FB_LIGHT_GREY, FB_BLACK);
    fb_write("=== Main Menu ===\n");
    fb_reset_color();
    fb_write("1) Math functions\n");
    fb_write("2) Color settings\n");
    fb_write("3) Clear screen\n");
    fb_write("4) Show examples again\n");
    fb_write("5) Exit to idle (do nothing)\n");
    fb_write("Select option: ");
}

/* ===================== kmain ===================== */

void kmain(void* mbd, unsigned int magic)
{
    (void)mbd;
    (void)magic;

    /* Interrupts */
    idt_install();
    pic_init();
    asm volatile("sti");

    /* Framebuffer setup */
    fb_enable_cursor(0, 15);
    fb_reset_color();
    fb_clear();

    /* Show initial examples */
    show_examples();

    /* Main loop: menu-driven */
    char buf[INPUT_BUF_SIZE];

    while (1) {
        show_main_menu();
        read_line(buf, INPUT_BUF_SIZE);

        if (buf[0] == '1') {
            math_menu();
        } else if (buf[0] == '2') {
            color_menu();
        } else if (buf[0] == '3') {
            fb_clear();
            fb_write("Screen cleared.\n\n");
        } else if (buf[0] == '4') {
            show_examples();
        } else if (buf[0] == '5') {
            fb_write("\nSystem is now idle. Press keys to see them echo:\n");
            while (1) {
                char c = keyboard_getchar();
                if (c) fb_putc(c);
            }
        } else {
            fb_set_color(FB_RED, FB_BLACK);
            fb_write("Unknown option. Try again.\n\n");
            fb_reset_color();
        }
    }
}
