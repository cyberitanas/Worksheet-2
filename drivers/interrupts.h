#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "../include/types.h"

/* CPU state pushed by ISR */
struct cpu_state {
    u32int eax, ebx, ecx, edx, ebp, esi, edi;
} __attribute__((packed));

/* Stack state pushed automatically */
struct stack_state {
    u32int error_code;
    u32int eip;
    u32int cs;
    u32int eflags;
} __attribute__((packed));

/* IDT entry */
struct idt_entry {
    u16int offset_low;
    u16int selector;
    u8int zero;
    u8int type_attr;
    u16int offset_high;
} __attribute__((packed));

/* IDT pointer */
struct idt_ptr {
    u16int limit;
    u32int base;
} __attribute__((packed));

/* Public API */
void interrupts_install_idt(void);
void interrupt_handler(struct cpu_state cpu,
                       u32int interrupt,
                       struct stack_state stack);

/* ASM hooks */
void load_idt(u32int);
void interrupt_handler_33(void);

#endif
