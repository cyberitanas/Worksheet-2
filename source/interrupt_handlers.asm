extern common_interrupt_handler

%macro ISR_NOERR 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword 0
    push dword %1
    jmp common_interrupt_handler
%endmacro

%macro ISR_ERR 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword %1
    jmp common_interrupt_handler
%endmacro

ISR_NOERR 32
ISR_NOERR 33
ISR_ERR 14
