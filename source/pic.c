#include "pic.h"
#include "../source/io.h"
#include "../include/types.h"

void pic_remap(s32int offset1, s32int offset2)
{
    /* Begin initialization */
    outb(PIC_1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    outb(PIC_2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);

    /* Set new offsets */
    outb(PIC_1_DATA, offset1);
    outb(PIC_2_DATA, offset2);

    /* Tell master PIC that slave PIC is at IRQ2 */
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
     * 0xFD = enable IRQ1 (keyboard), disable others
     * 0xFF = disable all on slave PIC
     */
    outb(PIC_1_DATA, 0xFD);
    outb(PIC_2_DATA, 0xFF);
}

void pic_acknowledge(u32int interrupt)
{
    if (interrupt >= PIC_2_OFFSET)
        outb(PIC_2_COMMAND, PIC_ACKNOWLEDGE);

    outb(PIC_1_COMMAND, PIC_ACKNOWLEDGE);
}
