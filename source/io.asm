global outb     ; Exported so C code can call this function
global inb      ; Exported so C code can call this function (added for Task 1)
global outw     ; Exported 16-bit write function



; ------------------------------------------------------------
; outb - send an 8-bit value to an I/O port
; C prototype:
;    void outb(unsigned short port, unsigned char data);
;
; (Provided in Worksheet 1 – included for completeness)
; ------------------------------------------------------------
outb:
    mov dx, [esp + 4]    ; Load I/O port number into DX
    mov al, [esp + 8]    ; Load 8-bit value into AL
    out dx, al           ; CPU instruction: write AL -> port DX
    ret                  ; Return to caller


; ------------------------------------------------------------
; inb - read an 8-bit value from an I/O port
; C prototype:
;    unsigned char inb(unsigned short port);
;
; *** ADDED FOR WORKSHEET 2 – TASK 1 ***
; This function is required so the OS can read input from hardware
; devices (e.g., keyboard uses I/O port 0x60). Without inb(), the
; OS would not be able to receive scancodes during interrupts.
; ------------------------------------------------------------
inb:
    mov dx, [esp + 4]    ; Load I/O port number into DX
    in  al, dx           ; CPU instruction: read byte from port DX -> AL
                         ; AL is the return value (C returns AL)
    ret                  ; Return to caller


; ------------------------------------------------------------
; outw - send a 16-bit value to an I/O port
; C prototype:
;    void outw(unsigned short port, unsigned short value);
;
; (Sometimes used for PIC configuration and QEMU shutdown)
; ------------------------------------------------------------
outw:
    mov dx, [esp + 4]    ; Load I/O port number
    mov ax, [esp + 8]    ; Load 16-bit value into AX
    out dx, ax           ; Write AX -> port DX
    ret                  ; Return to caller
