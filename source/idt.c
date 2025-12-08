#include "stdint.h"
#include "io.h"
#include "idt.h"

extern void keyboard_interrupt(void);   // from interrupts.asm

static struct idt_entry idt[256];
static struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low  = (uint16_t)(base & 0xFFFF);
    idt[num].base_high = (uint16_t)((base >> 16) & 0xFFFF);
    idt[num].selector  = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_install(void)
{
    // Set up IDT pointer
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    // Clear the table
    for (int i = 0; i < 256; i++) {
        idt[i].base_low  = 0;
        idt[i].base_high = 0;
        idt[i].selector  = 0;
        idt[i].always0   = 0;
        idt[i].flags     = 0;
    }

    // Install keyboard interrupt handler on vector 33 (IRQ1 + 32)
    idt_set_gate(33, (uint32_t)keyboard_interrupt, 0x08, 0x8E);

    // Load IDT
    __asm__ volatile("lidtl (%0)" : : "r" (&idtp));
}
