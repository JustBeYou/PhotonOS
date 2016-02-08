GLOBAL jmp_to_usermode

jmp_to_usermode:
    cli
    mov ax, 0x23
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax

    mov eax, esp
    push 0x23
    push eax
    pushf
    pop eax
    or eax, 0x200
    push eax
    push 0x1B
    push flush
    iret

flush:
