#include "pic.h"
#include "../source/io.h"

void pic_remap(s32int offset1, s32int offset2)
{
    /* Start initialization sequence */
    outb(PIC_1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    outb(PIC_2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);

    /* Set vector offsets */
    outb(PIC_1_DATA, offset1);  // usually 0x20
    outb(PIC_2_DATA, offset2);  // usually 0x28

    /* Tell PIC1 about PIC2 at IRQ2 */
    outb(PIC_1_DATA, 0x04);
    outb(PIC_2_DATA, 0x02);

    /* Set 8086 mode */
    outb(PIC_1_DATA, PIC_ICW4_8086);
    outb(PIC_2_DATA, PIC_ICW4_8086);

    /*
     * IRQ mask:
     * Bit = 1 → masked (disabled)
     * Bit = 0 → unmasked (enabled)
     *
     * 0xFD = 1111 1101b
     *        ^^^^^ ^ ^
     *        IRQ7..2 0 1
     *
     * So this masks everything except IRQ1 (keyboard).
     */
    outb(PIC_1_DATA, 0xFD);  // only IRQ1 enabled
    outb(PIC_2_DATA, 0xFF);  // mask all on slave PIC
}
