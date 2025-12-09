#include "../drivers/framebuffer.h"
#include "../drivers/keyboard.h"
#include "../source/idt.h"
#include "../source/pic.h"
#include "../source/io.h"

#define INPUT_BUF_SIZE 64

/* =====================================================
   BASIC UTILITY FUNCTIONS
   ===================================================== */

int sum_of_three(int a, int b, int c) {
    return a + b + c;
}

/* Blocking getchar */
static char read_char_blocking(void)
{
    char c;
    while ((c = keyboard_getchar()) == 0) {}
    return c;
}

/* Read a full line */
static int read_line(char *buf, int max_len)
{
    int len = 0;

    while (1) {
        char c = read_char_blocking();

        if (c == '\r') c = '\n';
        if (c == '\n') {
            fb_putc('\n');
            break;
        }

        if (c == '\b' || c == 127) {
            if (len > 0) {
                len--;
                int x = fb_get_cursor_x();
                int y = fb_get_cursor_y();
                if (x > 0) x--;
                else if (y > 0) { y--; x = FB_WIDTH - 1; }
                fb_set_cursor_pos(x, y);
                fb_putc(' ');
                fb_set_cursor_pos(x, y);
            }
            continue;
        }

        if (c >= 32 && c <= 126 && len < max_len - 1) {
            buf[len++] = c;
            fb_putc(c);
        }
    }

    buf[len] = '\0';
    return len;
}

/* Parse integer from string */
static int parse_int(const char *s, int *ok)
{
    int neg = 0, v = 0, i = 0;
    *ok = 0;

    if (s[0] == '-') { neg = 1; i = 1; }
    if (s[i] == '\0') return 0;

    for (; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return 0;
        v = v * 10 + (s[i] - '0');
    }

    if (neg) v = -v;
    *ok = 1;
    return v;
}

/* =====================================================
   SYSTEM TEST (previously "Show examples")
   ===================================================== */

static void system_test(void)
{
    fb_clear();
    fb_set_color(FB_CYAN, FB_BLACK);
    fb_draw_box(0, 0, FB_WIDTH, 3);
    fb_write_at(2, 1, "ItanasCyber OS  |  Student ID: 22035711");
    fb_reset_color();
    fb_set_cursor_pos(0, 4);

    fb_write("=== Running System Test ===\n\n");

    fb_set_color(FB_GREEN, FB_BLACK);
    fb_write("This is green text.\n");

    fb_set_color(FB_RED, FB_BLACK);
    fb_write("This is red text.\n");

    fb_set_color(FB_CYAN, FB_BLUE);
    fb_write("Cyan on blue OK.\n\n");

    fb_reset_color();
    fb_write("Integer test: ");
    fb_write_int(12345);
    fb_write("\nHex test: 0x");
    fb_write_hex(0xDEADBEEF);
    fb_write("\n\n");

    fb_write("Math test (5 + 3 + 2): ");
    fb_write_int(sum_of_three(5, 3, 2));
    fb_write("\n\n");

    fb_write("System test complete.\n\n");
}

/* =====================================================
   C Math Functions Menu
   ===================================================== */

static void math_menu(void)
{
    char buf[INPUT_BUF_SIZE];

    while (1) {
        fb_set_color(FB_YELLOW, FB_BLACK);
        fb_write("=== C Math Functions ===\n");
        fb_reset_color();

        fb_write("1) Sum of three numbers\n\n");
        fb_write("2) Subtract two numbers\n\n");
        fb_write("3) Multiply two numbers\n\n");
        fb_write("4) Back to main menu\n\n");
        fb_write("Select option: ");

        read_line(buf, INPUT_BUF_SIZE);

        if (buf[0] == '4') {
            fb_write("\nReturning...\n\n");
            return;
        }

        /* Sum */
        if (buf[0] == '1') {
            int a,b,c,ok1,ok2,ok3;

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
                fb_write("Invalid input.\n\n");
                fb_reset_color();
            } else {
                fb_set_color(FB_GREEN, FB_BLACK);
                fb_write("Result: ");
                fb_write_int(a + b + c);
                fb_write("\n\n");
                fb_reset_color();
            }
        }

        /* Subtract */
        else if (buf[0] == '2') {
            int a,b,ok1,ok2;

            fb_write("Enter minuend: ");
            read_line(buf, INPUT_BUF_SIZE);
            a = parse_int(buf, &ok1);

            fb_write("Enter subtrahend: ");
            read_line(buf, INPUT_BUF_SIZE);
            b = parse_int(buf, &ok2);

            if (!ok1 || !ok2) {
                fb_set_color(FB_RED, FB_BLACK);
                fb_write("Invalid input.\n\n");
            } else {
                fb_set_color(FB_GREEN, FB_BLACK);
                fb_write("Result: ");
                fb_write_int(a - b);
                fb_write("\n\n");
            }
            fb_reset_color();
        }

        /* Multiply */
        else if (buf[0] == '3') {
            int a,b,ok1,ok2;

            fb_write("Enter first number: ");
            read_line(buf, INPUT_BUF_SIZE);
            a = parse_int(buf, &ok1);

            fb_write("Enter second number: ");
            read_line(buf, INPUT_BUF_SIZE);
            b = parse_int(buf, &ok2);

            if (!ok1 || !ok2) {
                fb_set_color(FB_RED, FB_BLACK);
                fb_write("Invalid input.\n\n");
            } else {
                fb_set_color(FB_GREEN, FB_BLACK);
                fb_write("Result: ");
                fb_write_int(a * b);
                fb_write("\n\n");
            }
            fb_reset_color();
        }

        else {
            fb_set_color(FB_RED, FB_BLACK);
            fb_write("Unknown option.\n\n");
            fb_reset_color();
        }
    }
}

/* =====================================================
   Color Menu
   ===================================================== */

static void color_menu(void)
{
    char buf[INPUT_BUF_SIZE];

    while (1) {
        fb_set_color(FB_CYAN, FB_BLACK);
        fb_write("=== Color Settings ===\n");
        fb_reset_color();

        fb_write("1) White on Black\n\n");
        fb_write("2) Green on Black\n\n");
        fb_write("3) Cyan on Blue\n\n");
        fb_write("4) Yellow on Black\n\n");
        fb_write("5) Back to main menu\n\n");
        fb_write("Select option: ");

        read_line(buf, INPUT_BUF_SIZE);

        if (buf[0] == '5') {
            fb_write("\nReturning...\n\n");
            return;
        }

        if (buf[0] == '1') fb_set_color(FB_WHITE, FB_BLACK);
        else if (buf[0] == '2') fb_set_color(FB_GREEN, FB_BLACK);
        else if (buf[0] == '3') fb_set_color(FB_CYAN, FB_BLUE);
        else if (buf[0] == '4') fb_set_color(FB_YELLOW, FB_BLACK);
        else {
            fb_set_color(FB_RED, FB_BLACK);
            fb_write("Unknown option.\n\n");
            fb_reset_color();
            continue;
        }

        fb_write("\nColor updated.\n\n");
        return;
    }
}

/* =====================================================
   Header + Main Menu
   ===================================================== */

static void draw_header(void)
{
    fb_set_color(FB_CYAN, FB_BLACK);
    fb_draw_box(0, 0, FB_WIDTH, 3);
    fb_write_at(2, 1, "ItanasCyber OS  |  Student ID: 22035711");
    fb_reset_color();
    fb_set_cursor_pos(0, 4);
}

static void show_main_menu(void)
{
    fb_write("=== ItanasCyber OS Main Menu ===\n\n");

    fb_write("1) C Math Functions\n\n");
    fb_write("2) Color settings\n\n");
    fb_write("3) Clear screen\n\n");
    fb_write("4) Run system test\n\n");
    fb_write("5) Exit to idle mode\n\n");
    fb_write("6) Shutdown OS\n\n");

    fb_write("Select option: ");
}

/* =====================================================
   Shutdown
   ===================================================== */

static void shutdown_os(void)
{
    fb_write("\nShutting down...\n");

    outw(0x604, 0x2000);  // QEMU
    outw(0xB004, 0x2000); // Bochs fallback

    asm volatile("cli; hlt");
}

/* =====================================================
   kmain
   ===================================================== */

void kmain(void* mbd, unsigned int magic)
{
    (void)mbd; (void)magic;

    idt_install();
    pic_remap(0x20, 0x28);
    asm volatile("sti");

    fb_enable_cursor(0, 15);
    fb_reset_color();
    fb_clear();

    /* Draw header and menu ONLY */
    draw_header();

    char buf[INPUT_BUF_SIZE];

    while (1) {
        show_main_menu();
        read_line(buf, INPUT_BUF_SIZE);

        if (buf[0] == '1') math_menu();
        else if (buf[0] == '2') color_menu();
        else if (buf[0] == '3') {
            fb_clear();
            draw_header();
            fb_write("Screen cleared.\n\n");
        }
        else if (buf[0] == '4') system_test();
        else if (buf[0] == '5') {
            fb_write("\nIdle mode. Keys will echo below.\n");
            while (1) {
                char c = keyboard_getchar();
                if (c) fb_putc(c);
            }
        }
        else if (buf[0] == '6') shutdown_os();

        else {
            fb_set_color(FB_RED, FB_BLACK);
            fb_write("Invalid option.\n\n");
            fb_reset_color();
        }
        
    fb_clear();
    draw_header();

    }
}
