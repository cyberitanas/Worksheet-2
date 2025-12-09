global load_idt
global common_interrupt_handler

extern interrupt_handler

; -------------------------
; load_idt
; -------------------------
load_idt:
    mov eax, [esp+4]
    lidt [eax]
    ret

; -------------------------
; common_interrupt_handler
; -------------------------
common_interrupt_handler:

    ; Save GP registers
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi
    push edi

    ; Build stack_state*
    ; error_code, eip, cs, eflags
    push dword [esp+44] ; eflags
    push dword [esp+40] ; cs
    push dword [esp+36] ; eip
    push dword [esp+32] ; error_code

    ; Push interrupt number
    push dword [esp+48]

    ; Build cpu_state*
    push dword [esp+52] ; edi
    push dword [esp+52] ; esi
    push dword [esp+52] ; ebp
    push dword [esp+52] ; edx
    push dword [esp+52] ; ecx
    push dword [esp+52] ; ebx
    push dword [esp+52] ; eax

    call interrupt_handler

    add esp, 12*4

    pop edi
    pop esi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    iret
