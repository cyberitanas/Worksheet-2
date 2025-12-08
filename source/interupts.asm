global keyboard_interrupt
extern keyboard_handler

keyboard_interrupt:
    pusha
    call keyboard_handler
    popa
    iret
