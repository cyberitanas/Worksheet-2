section .text
global keyboard_interrupt
extern keyboard_handler

keyboard_interrupt:
    pusha                      ; Save registers

    call keyboard_handler      ; Call C handler

    mov al, 0x20               ; Send EOI to PIC1
    out 0x20, al

    popa                       ; Restore registers
    iretd                      ; Return from interrupt
