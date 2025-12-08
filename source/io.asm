global outb     ; Send byte to I/O port
global inb      ; Read byte from I/O port

; outb - send a byte to an I/O port
; stack: [esp + 8] data byte
;        [esp + 4] I/O port
;        [esp    ] return address
outb:
    mov al, [esp + 8]    ; Move data into AL
    mov dx, [esp + 4]    ; Move port into DX
    out dx, al           ; Send AL to port DX
    ret

; inb - read a byte from an I/O port
; stack: [esp + 4] I/O port
;        [esp    ] return address
inb:
    mov dx, [esp + 4]    ; Move port into DX
    in al, dx            ; Read from port DX into AL
    ret                  ; Return value in AL