
# build
make

# create ISO (Makefile should do this, but here are the commands if needed)
# cp kernel.elf iso/boot/
# cp -r grub/* iso/boot/grub/
# genisoimage -R -b "boot/grub/stage2_eltorito" -no-emul-boot -boot-load-size 4 -A "OS" -input-charset utf-8 -o os.iso iso/

# run qemu (text mode)
qemu-system-i386 -curses -monitor telnet::45454,server,nowait -serial mon:stdio -boot d -cdrom os.iso -m 32

# run qemu (nographic)
qemu-system-i386 -nographic -boot d -cdrom os.iso -m 32





# Worksheet 2 — Inputs & Interrupts

A short report describing the implementation for Worksheet 2 (Parts 1 & 2). This project implements a minimal Multiboot kernel, a framebuffer UI, basic C functions called from assembly, and an interrupt-driven keyboard input system.

**Build & Run (Quickstart)**

Prereqs: `make`, `qemu-system-i386`, `genisoimage` (or `mkisofs`)

```bash
# build the kernel and ISO (Makefile performs these steps)
make

# run in curses/text mode
qemu-system-i386 -curses -monitor telnet::45454,server,nowait \
    -serial mon:stdio -boot d -cdrom os.iso -m 32

# run in nographic mode (serial output)
qemu-system-i386 -nographic -boot d -cdrom os.iso -m 32
```

**What’s in this repo**

- `source/loader.asm`, `source/link.ld` — Multiboot entry + linker script
- `source/kmain.c` — kernel entry, shell, command handlers
- `drivers/framebuffer.c` — VGA text-mode driver (color, cursor, scrolling)
- `drivers/keyboard.c`, `drivers/input.c` — keyboard scan-code mapping + circular buffer
- `source/interrupts.c`, `source/pic.c`, `source/interrupt_asm.asm`, `source/interrupt_handlers.asm` — IDT, PIC remap, ISR stubs
- `source/io.asm` — `inb/outb/outw` port I/O helpers
- `iso/boot/grub/` — GRUB menu + stage2, used for building the ISO
- `screenshots/` — captures for the report

## Summary of features

- Multiboot-compatible loader that writes `0xCAFEBABE` to `EAX` and chains into C
- Custom linker script placing the kernel at 1 MiB (GRUB-friendly layout)
- Framebuffer driver with cursor control, colors, 2D helpers, and ANSI-free text output
- C math helpers callable from assembly (`sum_of_three`, `subtract_pair`, `multiply_pair`)
- Interrupt pipeline: PIC remap, IDT installation, NASM stubs, and a keyboard IRQ handler that feeds a circular buffer
- Tiny shell with `help`, `clear`, `echo`, math tests, and a `colours` palette demo

## 1. Task 1 — Bootloader, Linker Script, Minimal Kernel (Part 1)

The Multiboot header in `loader.asm` contains the required `MAGIC`, `FLAGS`, and `CHECKSUM`. GRUB loads the kernel at 0x00100000 (see `link.ld`). The loader sets `EAX = 0xCAFEBABE` before jumping to `kmain`, matching the worksheet verification step. Use `screenshots/task1_COMPLETE.png` or capture a fresh GRUB boot + log entry showing EAX if re-running.

Suggested screenshot: GRUB boot log or QEMU log showing `EAX=CAFEBABE` (from `logQ.txt`).

![GRUB + Multiboot verification](screenshots/task1_COMPLETE.png)
![Linker script placement](screenshots/link.ld.png)

## 2. Task 2 — Calling C from Assembly (Part 1)

`kmain` is invoked directly from `loader.asm` after setting up a stack. The shell exposes the required `sum_of_three` plus two additional helpers (`subtract_pair`, `multiply_pair`). Commands `sum`, `sub`, and `mul` print the results on screen. Suggested screenshot: the shell prompt running `sum`, `sub`, `mul` back-to-back.

![sum/sub/mul commands](screenshots/task2_COMPLETE.png)
![Additional math test](screenshots/task2_COMPLETE_2.png)
![Math menu UI](screenshots/math_menu.png)

## 3. Task 3 — Framebuffer Driver & UI (Part 1)

`drivers/framebuffer.c` talks to VGA text memory at 0xB8000, implements `fb_putc`, `fb_write`, integer formatting, cursor enable/disable, `fb_write_at`, `fb_draw_box`, etc. The new `colours` command demonstrates colour + background combinations in one screen. Suggested screenshot: run `colours` to show the palette output (captures both FG/BG requirements).

![Framebuffer UI](screenshots/task3_menu.png)
![Framebuffer primitives](screenshots/task3_COMPLETE.png)
![Colour palette demo](screenshots/task3_COMPLETE_2.png)
![Cursor control](screenshots/cursor.png)

Cursor control uses ports `0x3D4/0x3D5` (`fb_set_cursor_pos`), satisfying the I/O-port requirement.

## 4. Task 4 — README & Documentation (Part 1)

This README plus the screenshots folder covers the narrative, boot steps, framebuffer design, and demonstrates the OS running. Include a screenshot of the help text + student ID header to show context.

## 5. Task 1 — Keyboard Interrupts (Part 2)

`source/interrupts.c` configures the IDT and PIC (remap to 0x20/0x28), installs `interrupt_handler_33`, and unmask IRQ1. `source/interrupt_asm.asm` provides `load_idt` and the assembly stubs that save registers, build `cpu_state`/`stack_state`, and call the shared C handler. The handler drains port `0x60`, converts scan codes to ASCII (`drivers/keyboard.c`), updates the framebuffer, and pushes characters into the circular buffer. Suggested screenshot: the shell echoing keyboard input, showing backspace/newline handling.

![PIC remap walkthrough](screenshots/PIC.png)
![Keyboard IRQ demo](screenshots/menu.png)

## 6. Task 2 — Input Buffer API (Part 2)

`drivers/input.c` implements a 256-byte circular buffer with `input_reset`, `input_putc`, `input_backspace`, `getc`, and `readline`. The interrupt handler uses these APIs to feed keystrokes, and `readline` consumes them in `kmain`. Suggested screenshot: highlight the buffer code or show `readline` blocking/resuming (e.g., type a command, press backspace, show the corrected input).

## 7. Task 3 — Terminal / Shell (Part 2)

The `shell()` loop in `kmain.c` displays `myos>`, trims whitespace, and executes `help`, `clear`, `sum`, `sub`, `mul`, `echo`, and `colours`. Unknown commands return a helpful message. Suggested screenshot: the `help` output plus an `echo` example (already present in `screenshots/menu.png` or capture a new one).

## 8. Part 2 README Notes

The sections above describe the interrupt/keyboard/buffer flow. For final submission, include screenshots like:

1. **Boot verification:** GRUB + `Tiny OS started successfully.` banner (with Student ID `22035711`).
2. **C math menu:** `sum/sub/mul` outputs.
3. **Framebuffer colours:** `colours` command output showing FG/BG combinations.
4. **Keyboard interrupts:** typed command echoing, including backspace handling.
5. **QEMU log snippet:** optional, showing `EAX=0xCAFEBABE` if requested in viva.

Mention any constraints (e.g., limited screen space) and future extensions (history, tab completion) if desired.

## Worksheet 2 – Part 2: Inputs & Interrupts



## 1. Task 1 — Bootloader, Linker Script, and Minimal Kernel

### Bootloader (loader.asm)

The bootloader is implemented according to the Multiboot specification.
It declares:

* `MAGIC_NUMBER = 0x1BADB002`
* `FLAGS = 0x0`
* `CHECKSUM = -(MAGIC_NUMBER + FLAGS)`

These constants allow GRUB to recognise the kernel as a valid Multiboot OS image.

The `.text` section contains the entry point `loader:` which initially loads:

```asm
mov eax, 0xCAFEBABE
```
![alt text](screenshots/task1_COMPLETE.png)
This satisfies Task 1’s requirement to boot into an OS that sets `EAX = 0xCAFEBABE` and halts in an infinite loop. GRUB loads this at the physical address `0x00100000` as specified by the linker script.

### Linker Script (link.ld)

The linker script:
![alt text](screenshots/link.ld.png)

* places the kernel at the **1 MB boundary**, as GRUB requires
* aligns `.text`, `.rodata`, `.data`, and `.bss` sections to 4 KB boundaries
* defines the entry point `ENTRY(loader)`

This ensures the generated ELF file is compatible with Multiboot.

### ISO Construction

The Makefile:

* compiles the kernel
* copies `kernel.elf` into `iso/boot/`
* copies GRUB’s boot files into `iso/boot/grub/`
* embeds a valid `menu.lst` describing how GRUB boots the OS
* uses `genisoimage` to create `os.iso`

Running on QEMU using:

```
qemu-system-i386 -nographic -boot d -cdrom os.iso -m 32 -d cpu -D logQ.txt (this later changes in worksheet part 2)
```

allows validation that `EAX = 0xCAFEBABE`, satisfying Task 1 completely.
![alt text](screenshots/task1_COMPLETE.png)


## 2. Task 2 — Transition to C and Implementation of C Functions

### Calling C from Assembly

In `loader.asm`, instead of halting after writing to EAX, control is passed to C via:

```asm
extern kmain
call kmain
```

The stack and multiboot parameters are forwarded properly.

### sum_of_three() and Additional C Functions
![task2 complete!](/screenshots/task2_COMPLETE.png)

The required function:

```c
int sum_of_three(int a, int b, int c)
```
![alt text](mathmenu.png)
is implemented and used in the "System Test" menu.
Two additional C functions were added:

1. **Subtraction**
![task2 complete!](/screenshots/task2_COMPLETE_2.png)
Testing for calling C function to subtract 100 - 25 and write to eax in hex.

2. **Multiplication**

These are exposed in the C Math Menu of the kernel.

A full interactive menu system is written using the framebuffer API, allowing users to exercise the functions inside the running OS.
![alt text](screenshots/menu.png)


## 3. Task 3 — Framebuffer Driver, Cursor, Colors, and Enhanced UI

### Framebuffer Implementation

The framebuffer driver in `drivers/framebuffer.c` provides:

* direct memory-mapped I/O to address `0xB8000`
* `fb_putc()`, `fb_write()`, `fb_write_int()`, `fb_write_hex()`
* scrolling and cursor manipulation
* support for foreground/background colors

The cursor is controlled using I/O ports (`0x3D4`, `0x3D5`), implementing:

![alt text](cursor.png)

```c
void fb_set_cursor_pos(int x, int y);
void fb_enable_cursor(int start, int end);
```

### 2D API Implementation

The driver exposes a clear 2D drawing abstraction:

* `fb_draw_box()`
* `fb_write_at(x, y, text)`
* `fb_clear()`
* runtime color selection via menu

This satisfies the requirement for a full 2D framebuffer API.

### System Test Screen

The system test demonstrates all framebuffer features:
![alt text](screenshots/task3_COMPLETE_2.png)
* drawing a header box
* writing text in different colours
* printing integers and hex values
* verifying math functions

### QEMU Curses Mode

The OS runs in text mode using:

```
qemu-system-i386 -curses -monitor telnet::45454,server,nowait \
    -serial mon:stdio -boot d -cdrom os.iso -m 32
```

A second terminal provides telnet access to exit QEMU safely.



## 4. Task 4 — Interrupts, PIC, IDT, and Keyboard Input (Worksheet 2 Part 2)

### I/O Ports (inb / outb)

Per worksheet instructions, I implemented:

```asm
global inb
global outb
global outw
```

in `io.asm`, allowing the OS to interact with hardware.
These match the required prototypes:

```c
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);
void outw(unsigned short port, unsigned short value);
```

### PIC Remapping & Interrupt Enabling

The PIC driver (`pic.c`) implements:

* `pic_remap(0x20, 0x28)`
* masking/unmasking IRQs
* enabling IRQ1 for the keyboard
* acknowledging interrupts using EOI commands

This satisfies all PIC-related requirements.

### IDT and Interrupt Descriptors

My `interrupts.h` and `interrupts.c` implement:

* the IDT entry structure
* the IDT pointer
* CPU state & stack state structures
* `interrupts_install_idt()` to install handlers
* connection between assembly stubs and C dispatcher

### Assembly Interrupt Handlers

Two files were implemented:

#### interrupt_handlers.asm

Defines:

```
load_idt
```

to load the IDT register via `lidt`.

#### interrupt_asm.asm

Defines generic interrupt stubs and specific handlers:

* `interrupt_handler_33` (IRQ1 keyboard)
* `interrupt_handler_14` (page fault)

These push the interrupt number + error code and jump to the shared handler.

### Keyboard Driver

The keyboard driver:

* reads scancodes from I/O port `0x60`
* converts scancodes via lookup table to ASCII
* stores the last keypress into a buffer
* exposes `keyboard_getchar()` for non-blocking reads

### Integration in Kernel (kmain)

The kernel:

1. installs the IDT
2. installs interrupt handlers
3. initialises PIC / unmasks IRQ1
4. enables interrupts (`sti`)

Idle mode echoes keystrokes, demonstrating that the interruptdriven input system works.


## Worksheet 2 – Part 2: Inputs & Interrupts

Here is a revised version of the Worksheet 2 Part 2 README section, rewritten to sound much more natural and coursework-style, without the rigid “requirement / implementation / outcome” phrasing.
It reads like a student describing how they built the interrupt subsystem and how each part fits together in their OS.




# Worksheet 2 – Part 2: Inputs & Interrupts

This part of the worksheet focuses on adding keyboard input through hardware interrupts. The work builds directly on the framebuffer system developed earlier, but now the OS needs to respond to external hardware events rather than simply executing sequential C code. The implementation follows the structure described in Chapter 6 of the OS handbook, introducing interrupt handlers, the PIC, an IDT, and a basic keyboard driver.



## Extending the I/O Library

The first step was to extend the low level I/O library so the OS can *read* from hardware ports. Up to this point I had only implemented output instructions, which were enough for the framebuffer. Keyboard input, however, requires using the `in` CPU instruction. I added this as a new function (`inb`) inside `io.asm`, and declared it in `io.h`. This gives the rest of the kernel a simple C interface for interacting with I/O ports like `0x60`, which is where the keyboard places its scancodes.






## Setting Up the PIC

Handling interrupts correctly requires reconfiguring the Programmable Interrupt Controller (PIC). By default, the PIC maps IRQs onto interrupt vectors that clash with CPU exceptions, so they need to be remapped to safer offsets. I implemented a dedicated PIC driver (`pic.c`) that performs this remapping and sets up the controller in 8086 mode. The driver also handles interrupt masking, and I specifically enabled IRQ1 since that is the keyboard interrupt.

Once the PIC is initialised, IRQ1 appears at interrupt vector **33 (0x21)**, which matches the convention used later when installing IDT entries.


## Building the Interrupt Descriptor Table

The next required step was creating the IDT and all the supporting structures. These are defined in `interrupts.h`, including the IDT entry format and the saved CPU/stack state that is passed into the C-level interrupt handler.

The IDT installation logic is implemented in `interrupts.c`. Here I set up the descriptor table and install the handlers for the interrupts I actually use, such as:

* Interrupt 33 → keyboard IRQ (handled through my keyboard driver)
* Interrupt 14 → page fault (included for completeness, matching the worksheet)

The IDT is then loaded through the `load_idt` assembly function, which I implemented in `interrupt_handlers.asm` following the lid instruction pattern from the worksheet.



## Assembly Interrupt Handlers

To connect the CPU’s interrupts to the C interrupt handler, I wrote the second assembly file, `interrupt_asm.asm`. This contains generic interrupt stubs generated using macros, as described in the worksheet. These stubs:

1. Push the interrupt number (and error code, when needed),
2. Save the register state,
3. And then jump into the shared C interrupt handler.

This matches the model described in the handbook, where assembly captures the CPU state and C performs the actual work.



## C-Level Interrupt Handler

The OS eventually ends up in the C interrupt dispatcher in `interrupts.c`, where I check which interrupt fired. For interrupt 33, the keyboard case, I call:

```c
keyboard_handle_irq();
```

After handling an interrupt, the dispatcher always acknowledges it by sending the EOI command to the PIC, allowing future interrupts to arrive. This acknowledgement step is essential—without it, hardware interrupts stop firing.



## Keyboard Driver

To complete the interrupt-driven input system, I implemented a keyboard driver in `drivers/keyboard.c`. It reads raw scancodes from port `0x60` using the `inb` function added earlier. Only key presses are processed (the driver ignores release codes), and scancodes are mapped to ASCII values using a lookup table. The driver stores the latest key in a single-byte buffer, which the kernel can retrieve through a simple non-blocking `keyboard_getchar()` function.

This approach gives the OS a minimal but functional input system fully powered by hardware interrupts.



## Integration into the Kernel

All of the above components are activated during kernel startup in `kmain.c`. Once the framebuffer is initialised, the kernel:

1. Installs the IDT,
2. Sets up the PIC,

From that point on, keyboard input is interrupt-driven. In idle mode the kernel simply echoes keystrokes as they arrive, which demonstrates that:

* The PIC has been correctly remapped,
* The interrupt stubs are working,
* The C dispatcher is receiving the interrupt number,
* And the keyboard driver is decoding scancodes correctly.
