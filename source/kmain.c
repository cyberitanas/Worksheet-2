#include "../drivers/framebuffer.h"
#include "../drivers/input.h"
#include "interrupts.h"

#define INPUT_BUF_SIZE 128

static int strings_equal(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b) {
            return 0;
        }
        ++a;
        ++b;
    }
    return (*a == '\0' && *b == '\0');
}

static void trim_line(char *line)
{
    int start = 0;
    while (line[start] == ' ' || line[start] == '\t' || line[start] == '\r') {
        start++;
    }

    int i = 0;
    if (start > 0) {
        while (line[start + i]) {
            line[i] = line[start + i];
            i++;
        }
        line[i] = '\0';
    }

    int len = 0;
    while (line[len]) {
        len++;
    }
    while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t' || line[len - 1] == '\r')) {
        len--;
    }
    line[len] = '\0';
}

static int starts_with(const char *str, const char *prefix)
{
    while (*prefix) {
        if (*str != *prefix) {
            return 0;
        }
        ++str;
        ++prefix;
    }
    return 1;
}

/* =====================================================
   Simple test function (Worksheet Part 1 – Task 2)
   ===================================================== */
int sum_of_three(int a, int b, int c)
{
    return a + b + c;
}

int subtract_pair(int a, int b)
{
    return a - b;
}

int multiply_pair(int a, int b)
{
    return a * b;
}

/* =====================================================
   Simple command handlers
   ===================================================== */
static void cmd_help(void)
{
    fb_write("Available commands:\n");
    fb_write("  help    - show this message\n");
    fb_write("  clear   - clear the screen\n");
    fb_write("  echo X  - print text X\n");
    fb_write("  sum     - test C function call\n");
    fb_write("  sub     - run subtract_pair test\n");
    fb_write("  mul     - run multiply_pair test\n");
    fb_write("  colours - show framebuffer colours\n");
}

static void cmd_sum(void)
{
    fb_write("sum_of_three(2, 3, 4) = ");
    fb_write_int(sum_of_three(2, 3, 4));
    fb_write("\n");
}

static void cmd_sub(void)
{
    fb_write("subtract_pair(10, 3) = ");
    fb_write_int(subtract_pair(10, 3));
    fb_write("\n");
}

static void cmd_mul(void)
{
    fb_write("multiply_pair(6, 7) = ");
    fb_write_int(multiply_pair(6, 7));
    fb_write("\n");
}

static void cmd_colours(void)
{
    const char *fg_names[] = {
        "BLACK", "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN", "LIGHT_GREY",
        "DARK_GREY", "LIGHT_BLUE", "LIGHT_GREEN", "LIGHT_CYAN",
        "LIGHT_RED", "LIGHT_MAGENTA", "LIGHT_BROWN", "WHITE"
    };

    for (uint8_t fg = 0; fg < 16; ++fg) {
        fb_set_color(fg, FB_BLACK);
        fb_write("FG ");
        fb_write_int(fg);
        fb_write(" (");
        fb_write(fg_names[fg]);
        fb_write(")\n");
    }

    fb_write("\n");
    fb_write("Background examples:\n");
    for (uint8_t bg = 0; bg < 8; ++bg) {
        fb_set_color(FB_BLACK, bg);
        fb_write("BG ");
        fb_write_int(bg);
        fb_write(" sample text\n");
    }

    fb_reset_color();
    fb_set_cursor_pos(0, fb_get_cursor_y() + 1);
}

/* =====================================================
   Simple terminal (Worksheet Part 2 – Task 3)
   ===================================================== */
static void shell(void)
{
    char line[INPUT_BUF_SIZE];

    while (1) {
        fb_write("myos> ");
        readline(line, INPUT_BUF_SIZE);

        trim_line(line);

        if (line[0] == '\0') {
            continue;
        }
        else if (strings_equal(line, "help")) {
            cmd_help();
        }
        else if (strings_equal(line, "clear")) {
            fb_clear();
        }
        else if (strings_equal(line, "sum")) {
            cmd_sum();
        }
        else if (strings_equal(line, "sub")) {
            cmd_sub();
        }
        else if (strings_equal(line, "mul")) {
            cmd_mul();
        }
        else if (strings_equal(line, "colours")) {
            cmd_colours();
        }
        else if (starts_with(line, "echo")) {
            const char *arg = line + 4;
            while (*arg == ' ' || *arg == '\t')
                ++arg;
            fb_write(arg);
            fb_write("\n");
        }
        else {
            fb_write("Unknown command. Type 'help'.\n");
        }
    }
}

/* =====================================================
   Kernel entry point
   ===================================================== */
void kmain(void)
{
    input_reset();

    /* Install interrupts */
    interrupts_install_idt();
    asm volatile("sti");

    /* Initialise framebuffer */
    fb_clear();
    fb_write("Tiny OS started successfully.\n");
    fb_write("Student ID: 22035711\n");
    fb_write("Type 'help' to see available commands.\n\n");

    /* Start terminal */
    shell();
}
