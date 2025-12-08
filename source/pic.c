#include "../source/io.h"
#include "pic.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void pic_init() {
    /* Start initialization */
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    /* Remap IRQs: PIC1 -> 0x20–0x27, PIC2 -> 0x28–0x2F */
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    /* Configure cascading */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    /* Environment info */
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    /* Unmask IRQ1 (keyboard) on PIC1 */
    outb(PIC1_DATA, 0xFD);   // 1111 1101 (IRQ1 enabled)
    
    /* Mask all IRQs on PIC2 */
    outb(PIC2_DATA, 0xFF);
}

void pic_acknowledge(unsigned int irq) {
    (void)irq;
    outb(PIC1_COMMAND, 0x20);
}
