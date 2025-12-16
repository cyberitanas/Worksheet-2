#include "interrupts.h"
#include "framebuffer.h"
#include "../source/pic.h"
#include "../source/io.h"
#include "keyboard.h"
#include "input.h"

#define IDT_SIZE 256
#define IRQ_KEYBOARD 33
#define KEYBOARD_STATUS_PORT 0x64

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

/* Create an IDT entry */
static void idt_set_gate(int n, u32int handler)
{
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;     // kernel code segment
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;     // present, ring 0, 32-bit interrupt gate
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void interrupts_install_idt(void)
{
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (u32int)&idt;

    for (int i = 0; i < IDT_SIZE; ++i) {
        idt[i].offset_low = 0;
        idt[i].offset_high = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
    }

    /* Keyboard interrupt */
    idt_set_gate(IRQ_KEYBOARD, (u32int)interrupt_handler_33);

    load_idt((u32int)&idtp);

    /* Remap PIC and unmask keyboard */
    pic_remap(0x20, 0x28);
    outb(PIC_1_DATA, inb(PIC_1_DATA) & ~(1 << 1));
}

/* Common interrupt handler called from ASM */
void interrupt_handler(struct cpu_state cpu,
                       u32int interrupt,
                       struct stack_state stack)
{
    (void)cpu;
    (void)stack;

    if (interrupt == IRQ_KEYBOARD) {
        while (inb(KEYBOARD_STATUS_PORT) & 1) {
            u8int scan_code = keyboard_read_scan_code();
            u8int ascii = keyboard_scan_code_to_ascii(scan_code);

            if (!ascii) {
                continue;
            }

            if (ascii == '\b') {
                fb_backspace();
                input_backspace();
            } else if (ascii == '\n') {
                fb_newline();
                input_putc('\n');
            } else {
                fb_putc((char)ascii);
                input_putc((char)ascii);
            }
        }
    }

    pic_acknowledge(interrupt);
}
