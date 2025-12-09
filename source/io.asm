global outb     ; Send byte to I/O port
global inb      ; Read byte from I/O port
global outw     ; Send 16-bit word to I/O port

; ------------------------------------------------------------
; outb - send an 8-bit value to an I/O port
; C prototype:
;    void outb(unsigned short port, unsigned char data);
; ------------------------------------------------------------
outb:
    mov dx, [esp + 4]    ; port
    mov al, [esp + 8]    ; 8-bit data
    out dx, al
    ret

; ------------------------------------------------------------
; inb - read an 8-bit value from an I/O port
; C prototype:
;    unsigned char inb(unsigned short port);
; ------------------------------------------------------------
inb:
    mov dx, [esp + 4]    ; port
    in  al, dx           ; read byte into AL
    ret                  

; ------------------------------------------------------------
; outw - send a 16-bit value to an I/O port
; C prototype:
;    void outw(unsigned short port, unsigned short value);
; ------------------------------------------------------------
outw:
    mov dx, [esp + 4]    ; port
    mov ax, [esp + 8]    ; 16-bit value
    out dx, ax
    ret
