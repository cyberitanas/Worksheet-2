#include "../drivers/framebuffer.h"
#include "../drivers/keyboard.h"
#include "../source/idt.h"
#include "../source/pic.h"

/* Simple test function */
int sum_of_three(int a, int b, int c) {
    return a + b + c;
}

void kmain(void* mbd, unsigned int magic)
{
    (void)mbd;
    (void)magic;

    /* ===================== INTERRUPTS ===================== */
    idt_install();           // Load IDT and install handlers
    pic_init();              // Remap PIC and unmask IRQ1 (keyboard)
    asm volatile("sti");     // Enable interrupts

    /* ===================== FRAMEBUFFER SETUP ===================== */
    fb_enable_cursor(0, 15);
    fb_set_color(FB_WHITE, FB_BLACK);
    fb_clear();

    /* ===================== UI HEADER ===================== */
    fb_set_color(FB_CYAN, FB_BLACK);
    fb_draw_box(0, 0, FB_WIDTH, 3);
    fb_write_at(2, 1, "Welcome to Tiny OS!");
    fb_set_cursor_pos(0, 4);

    /* ===================== COLOR TESTS ===================== */
    fb_set_color(FB_GREEN, FB_BLACK);
    fb_write("This is green text\n");

    fb_set_color(FB_RED, FB_BLACK);
    fb_write("This is red text\n");

    fb_set_color(FB_CYAN, FB_BLUE);
    fb_write("Cyan on blue!\n\n");

    fb_reset_color();

    /* ===================== NUMBER TESTS ===================== */
    fb_write("Testing numbers: ");
    fb_write_int(12345);
    fb_write("\n");

    fb_write("Hex value: ");
    fb_write_hex(0xDEADBEEF);
    fb_write("\n\n");

    /* ===================== MATH TEST ===================== */
    fb_write("Math tests:\n");
    fb_write("5 + 3 + 2 = ");
    fb_write_int(sum_of_three(5, 3, 2));
    fb_write("\n\n");

    /* ===================== KEYBOARD INPUT ===================== */
    fb_set_color(FB_YELLOW, FB_BLACK);
    fb_write("Keyboard test (interrupt driven):\n");
    fb_write("Type something:\n> ");
    fb_reset_color();

    /* ===================== MAIN LOOP ===================== */
    while (1)
    {
        char c = keyboard_getchar();
        if (c)
            fb_putc(c);
    }
}
