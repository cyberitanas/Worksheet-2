Task 1 - 

![task1 complete!](/screenshots/task1_COMPLETE.png)

Task 2 - 


Test for calling C function to add 5 + 10 + 15 and write it to eax in hex.
![task2 complete!](/screenshots/task2_COMPLETE.png)


![task2 complete!](/screenshots/task2_COMPLETE_2.png)
Testing for calling C function to subtract 100 - 25 and write to eax in hex.

Task 3 - 
![task3 complete!](/screenshots/task3_COMPLETE.png)
![task3 complete!](/screenshots/task3_COMPLETE_2.png)
![task3 complete!](/screenshots/task3_COMPLETE_3.png)
![task3 complete!](/screenshots/task3_COMPLETE_3.png)
![task3 menu pic](screenshots/task3_menu.png)

; ======================================================================
; Worksheet 2 – Task 1: Extending the I/O Library
; ----------------------------------------------------------------------
; Task 1 required adding support for reading from hardware I/O ports.
; In Worksheet 1, the OS could only write to ports using outb/outw.
; To support keyboard input, this worksheet introduces the 'inb'
; function, which enables the kernel to read 8-bit values directly
; from devices such as the keyboard controller at I/O port 0x60.
;
; This addition is fundamental because the CPU's IN/OUT instructions
; cannot be performed in C; therefore, lightweight assembly stubs
; must expose these operations to the kernel. The inb interface is
; used later by the interrupt-driven keyboard handler.
; ======================================================================


; ======================================================================
; Worksheet 2 – Task 2: Type Definitions & PIC Configuration
; ----------------------------------------------------------------------
; Task 2 required creating a 'types.h' file to provide consistent,
; fixed-size integer types (u8int, u16int, u32int, etc.) used across
; the kernel. This ensures predictable data sizes when interfacing
; with hardware and low-level system components.
;
; The task also required implementing the Programmable Interrupt
; Controller (PIC) setup through 'pic.h' and 'pic.c'. The PIC must be
; remapped because, by default, IRQs overlap CPU exception vectors.
; Interrupts 0–15 are relocated to 0x20–0x2F, preventing conflicts and
; allowing the OS to assign clean IDT entries.
;
; The PIC remapping function configures both master and slave PICs
; using Initialization Command Words (ICWs), and the acknowledge
; function signals end-of-interrupt (EOI) after each ISR.



PIC Remapping Verification (README Paragraph)

To implement and verify correct PIC remapping, I followed the procedure described in the PIC resource, which outlines why the legacy IRQ vector range (0–15) must be shifted to new offsets to avoid conflicts with CPU exceptions. Using this guidance, I remapped PIC1 to interrupt vectors 0x20–0x27 and PIC2 to 0x28–0x2F, and implemented the corresponding acknowledgement logic where each handled interrupt must send the command byte 0x20 back to the appropriate PIC. This behaviour was directly informed by the resource’s explanation that unacknowledged interrupts cause the PIC to stop issuing further requests. I also drew on the conceptual explanations in The Little Book About OS Development (Helin and Renberg, 2015), particularly its emphasis on building small, verifiable components when bringing up early kernel subsystems. That approach guided how I incrementally tested the interrupt path—first by enabling port input via inb, then by wiring the keyboard IRQ through the remapped vector table, and finally by inserting a temporary framebuffer debug print inside keyboard_handler(). Since IRQ1 should appear at vector 0x21 after remapping, the on-screen message “IRQ1 → INT33” confirmed that the IDT entry, remapped PIC offsets, and acknowledgement pathway were all functioning consistently with the mechanisms described in both the PIC documentation and Helin and Renberg’s discussion of interrupt-driven hardware interaction.



Helin, E. and Renberg, A., 2015. The little book about OS development.

![alt text](<screenshots/IO port into AL.png>)

![alt text](<screenshots/IO port into AL_2.png>)

; ======================================================================


; ======================================================================
; Worksheet 2 – Task 3: Interrupt Descriptor Table (IDT) & Handlers
; ----------------------------------------------------------------------
; Task 3 required building the infrastructure for interrupt handling.
; This includes defining the IDT structures, installing IDT entries,
; creating assembly-level stubs for each interrupt, and implementing
; a central C-level interrupt dispatcher.
;
; The IDT entries map hardware interrupts (IRQs) and CPU exceptions
; to handler routines. For this worksheet, key interrupts include
; interrupt 33 (IRQ1 – keyboard) and interrupt 14 (page fault).
;
; Assembly stubs capture CPU state, push error codes when required,
; and forward execution to 'interrupt_handler' in C. The dispatcher
; determines which interrupt has occurred and performs the appropriate
; action—such as reading scancodes for keyboard input—before sending
; an EOI to the PIC.
;
; Constructing the IDT is essential, as the CPU relies on these
; descriptors to safely transfer control during hardware events.
; ======================================================================


; ======================================================================
; Worksheet 2 – Documentation Requirement (README)
; ----------------------------------------------------------------------
; The final component of the worksheet requires documenting all work
; in README.md. This includes explaining the purpose of the I/O
; library extension, the design of the PIC remapping process, the
; structure of the IDT, and the flow of control during an interrupt.
;
; The README must clearly describe how keyboard input is captured
; via IRQ1, how the interrupt handler processes scancodes, and how
; evidence was gathered to demonstrate correct system behaviour
; (e.g., proving that IRQ1 triggers interrupt vector 33).
;
; This written component is an assessed part of Worksheet 2 and is
; worth 20% of its total grade.
; ======================================================================




PIC Remapping Verification (README Paragraph)

Task 1 involved extending the kernel’s low-level I/O library to support reading data from hardware devices. In Worksheet 1, only output operations were required (outb/outw), which allowed the OS to write values to I/O ports such as the VGA controller. However, implementing keyboard input in this worksheet requires the ability to read from device ports, particularly port 0x60, where the keyboard controller places its scancodes. To enable this functionality, an assembly routine inb was added to the I/O subsystem and its prototype was declared in io.h. The inb function exposes the CPU’s IN instruction to C, allowing the kernel’s interrupt handlers to retrieve hardware input during an IRQ. This addition completes the I/O interface needed for receiving keyboard input and forms the foundation for the interrupt-driven input system implemented in later tasks
![alt text](<screenshots/IO port into AL.png>)

To verify that the Programmable Interrupt Controller (PIC) was successfully remapped from its legacy interrupt vectors (0x08–0x0F) to the new offsets 0x20 and 0x28, a debug message was temporarily inserted inside the keyboard interrupt handler (IRQ1). After remapping, IRQ1 is expected to appear at interrupt vector 33 (0x21). By adding a framebuffer print statement directly inside keyboard_handler(), the system outputs a confirmation message whenever the keyboard interrupt fires. Upon booting the OS and pressing a key, the message “IRQ1 → INT33 (PIC remap OK)” is displayed on screen. This behaviour demonstrates that IRQ1 is no longer delivered on its original vector, and instead correctly arrives at interrupt 33, confirming that the PIC remap procedure executed correctly and that the IDT entry for the remapped IRQ is functioning as intended.

![alt text](<screenshots/IO port into AL_2.png>)