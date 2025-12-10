#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

#include "../include/types.h"

/* I/O port bases */
#define PIC_1              0x20
#define PIC_2              0xA0

#define PIC_1_COMMAND      PIC_1
#define PIC_1_DATA         (PIC_1 + 1)

#define PIC_2_COMMAND      PIC_2
#define PIC_2_DATA         (PIC_2 + 1)

/* Remapped interrupt offsets */
#define PIC_1_OFFSET       0x20   // IRQ0–7 -> INT 0x20–0x27
#define PIC_2_OFFSET       0x28   // IRQ8–15 -> INT 0x28–0x2F

/* PIC acknowledge command */
#define PIC_ACKNOWLEDGE    0x20

/* Initialization control words */
#define PIC_ICW1_ICW4      0x01
#define PIC_ICW1_SINGLE    0x02
#define PIC_ICW1_INTERVAL4 0x04
#define PIC_ICW1_LEVEL     0x08
#define PIC_ICW1_INIT      0x10

#define PIC_ICW4_8086      0x01
#define PIC_ICW4_AUTO      0x02
#define PIC_ICW4_BUF_SLAVE 0x08
#define PIC_ICW4_BUF_MASTER 0x0C
#define PIC_ICW4_SFNM      0x10

void pic_remap(s32int offset1, s32int offset2);
void pic_acknowledge(u32int interrupt);

#endif
