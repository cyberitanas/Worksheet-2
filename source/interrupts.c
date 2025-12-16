#include "interrupts.h"
#include "pic.h"
#include "io.h"
#include "../drivers/framebuffer.h"
#include "../drivers/input.h"
#include "../drivers/keyboard.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256
#define INTERRUPTS_KEYBOARD 33
#define KEYBOARD_STATUS_PORT 0x64

static struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];
static struct IDT idt;

static void interrupts_clear_descriptor(int index)
{
    idt_descriptors[index].offset_low = 0;
    idt_descriptors[index].offset_high = 0;
    idt_descriptors[index].segment_selector = 0;
    idt_descriptors[index].reserved = 0;
    idt_descriptors[index].type_and_attr = 0;
}

static void interrupts_init_descriptor(int index, u32int address)
{
    idt_descriptors[index].offset_low = address & 0xFFFF;
    idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF;
    idt_descriptors[index].segment_selector = 0x08;
    idt_descriptors[index].reserved = 0x00;
    idt_descriptors[index].type_and_attr = (1 << 7) | 0x0E;
}

void interrupts_install_idt(void)
{
    for (int i = 0; i < INTERRUPTS_DESCRIPTOR_COUNT; ++i) {
        interrupts_clear_descriptor(i);
    }

    interrupts_init_descriptor(INTERRUPTS_KEYBOARD, (u32int)interrupt_handler_33);

    idt.address = (u32int)&idt_descriptors;
    idt.size = sizeof(struct IDTDescriptor) * INTERRUPTS_DESCRIPTOR_COUNT - 1;

    load_idt((u32int)&idt);

    pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);

    /* Unmask keyboard IRQ (IRQ1) */
    outb(PIC_1_DATA, inb(PIC_1_DATA) & ~(1 << 1));
}

void interrupt_handler(__attribute__((unused)) struct cpu_state cpu,
                       u32int interrupt,
                       __attribute__((unused)) struct stack_state stack)
{
    if (interrupt == INTERRUPTS_KEYBOARD) {
        while (inb(KEYBOARD_STATUS_PORT) & 1) {
            u8int scan_code = keyboard_read_scan_code();
            u8int ascii = keyboard_scan_code_to_ascii(scan_code);

            if (ascii == 0) {
                continue;
            }

            if (ascii == '\b') {
                fb_backspace();
                input_putc('\b');
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
