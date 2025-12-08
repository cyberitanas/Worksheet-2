#include "../drivers/framebuffer.h"

// Added function
int sum_of_three(int a, int b, int c) {
    return a + b + c;
}

void kmain(void* mbd, unsigned int magic) 
{
    (void)mbd;
    (void)magic;
    
    // Clear screen
    fb_clear();
    
    // Test basic writing
    fb_write("Welcome to Tiny OS!\n");
    fb_write("===================\n\n");
    
    // Test colors
    fb_set_color(FB_GREEN, FB_BLACK);
    fb_write("This is green text\n");
    
    fb_set_color(FB_RED, FB_BLACK);
    fb_write("This is red text\n");
    
    fb_set_color(FB_CYAN, FB_BLUE);
    fb_write("Cyan on blue!\n\n");
    
    // Reset to default
    fb_set_color(FB_WHITE, FB_BLACK);
    
    // Test numbers
    fb_write("Testing numbers: ");
    fb_write_int(12345);
    fb_write("\n");
    
    fb_write("Hex value: ");
    fb_write_hex(0xDEADBEEF);
    fb_write("\n\n");
    
    // Test cursor positioning
    fb_move(10, 10);
    fb_write("Positioned text!");
    
    fb_move(0, 12);
    fb_write("Back to normal flow\n");
    
    // Test functions from Task 2
    fb_write("\nMath tests:\n");
    fb_write("5 + 3 + 2 = ");
    fb_write_int(sum_of_three(5, 3, 2));
    fb_write("\n");
    
    while (1);
}
