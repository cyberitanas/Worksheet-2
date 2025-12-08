#include "stdint.h"
#include "io.h"
#include "idt.h"
extern void keyboard_interrupt(void);

void idt_install(void)
{
    idt_set_gate(33, (unsigned)keyboard_interrupt, 0x08, 0x8E);
}
