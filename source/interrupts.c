#include "interrupts.h"
#include "../include/types.h"
#include "../drivers/keyboard.h"
#include "pic.h"
#include <stdint.h>

/*
 * C-level interrupt dispatcher.
 * Called from common_interrupt_handler in assembly.
 */
void interrupt_handler(struct cpu_state* cpu, u32int interrupt, struct stack_state* stack)
{
    (void)cpu;
    (void)stack;

    switch (interrupt)
    {
        case 33: // IRQ1 – Keyboard
            keyboard_handler();
            break;

        case 14: // Page fault
            // Worksheet: do nothing
            break;

        default:
            // Unknown interrupt
            break;
    }

    // ALWAYS send End-Of-Interrupt
    pic_acknowledge(interrupt);
}

/*
 * ISR stubs from interrupt_asm.asm
 */
extern void interrupt_handler_32(); // Timer IRQ
extern void interrupt_handler_33(); // Keyboard IRQ
extern void interrupt_handler_14(); // Page Fault

/*
 * Provided by idt.c
 */
extern void idt_set_gate(u8int num, uintptr_t base, u16int sel, u8int flags);

/*
 * Install IDT entries for interrupts we care about
 */
void interrupts_install_idt()
{
    idt_set_gate(32, (uintptr_t)interrupt_handler_32, 0x08, 0x8E); // Timer
    idt_set_gate(33, (uintptr_t)interrupt_handler_33, 0x08, 0x8E); // Keyboard
    idt_set_gate(14, (uintptr_t)interrupt_handler_14, 0x08, 0x8E); // Page fault
}
