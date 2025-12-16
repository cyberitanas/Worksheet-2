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

    mov esi, esp            ; remember base pointer to saved regs
    mov ebx, esp
    add ebx, 28             ; EBX -> start of pushed interrupt number

    ; Push stack_state (error_code, eip, cs, eflags)
    push dword [ebx+16]     ; eflags
    push dword [ebx+12]     ; cs
    push dword [ebx+8]      ; eip
    push dword [ebx+4]      ; error code

    ; Push interrupt number
    push dword [ebx]

    ; Push cpu_state fields (eax, ebx, ecx, edx, ebp, esi, edi)
    push dword [esi+24]     ; eax
    push dword [esi+20]     ; ebx
    push dword [esi+16]     ; ecx
    push dword [esi+12]     ; edx
    push dword [esi+8]      ; ebp
    push dword [esi+4]      ; esi
    push dword [esi]        ; edi

    call interrupt_handler

    add esp, 12*4

    pop edi
    pop esi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    add esp, 8              ; discard interrupt number + error code

    iret
