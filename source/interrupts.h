#ifndef INCLUDE_INTERRUPTS_H
#define INCLUDE_INTERRUPTS_H

#include "../include/types.h"

/* -----------------------------------------
   IDT Pointer (passed to lidt)
----------------------------------------- */
struct IDT {
    u16int size;
    u32int address;
} __attribute__((packed));

/* -----------------------------------------
   IDT Descriptor Entry
----------------------------------------- */
struct IDTDescriptor {
    u16int offset_low;
    u16int segment_selector;
    u8int  reserved;
    u8int  type_and_attr;
    u16int offset_high;
} __attribute__((packed));

/* -----------------------------------------
   CPU register snapshot (pushed by ASM)
----------------------------------------- */
struct cpu_state {
    u32int eax;
    u32int ebx;
    u32int ecx;
    u32int edx;
    u32int ebp;
    u32int esi;
    u32int edi;
} __attribute__((packed));

/* -----------------------------------------
   Hardware-pushed stack snapshot
----------------------------------------- */
struct stack_state {
    u32int error_code;
    u32int eip;
    u32int cs;
    u32int eflags;
} __attribute__((packed));

/* -----------------------------------------
   C interrupt dispatcher
----------------------------------------- */
void interrupt_handler(struct cpu_state* cpu, u32int interrupt, struct stack_state* stack);

/* -----------------------------------------
   Install IDT entries (interrupts.c)
----------------------------------------- */
void interrupts_install_idt();

/* -----------------------------------------
   External ASM functions
----------------------------------------- */
void load_idt(u32int);
void interrupt_handler_32();
void interrupt_handler_33();
void interrupt_handler_14();

#endif
