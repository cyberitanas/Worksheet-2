#include "../source/io.h"
#include "pic.h"

#define PIC1_COMMAND 0x20

void pic_acknowledge(unsigned int irq)
{
    (void)irq;
    outb(PIC1_COMMAND, 0x20);
}
