extern num_syscalls
extern syscalls

[global syscall_handler]
syscall_handler:
    ; save registers
    push eax
    push ebx
    push ecx
    push edx

    ; load registers_t
    mov eax, [esp + 20]     ; *regs
    mov ebx, [eax + 52]     ; regs->eax
    mov ecx, [num_syscalls] ; num_syscalls

    ; test if regs.eax >= num_syscalls
    cmp ebx, ecx
    jge syscall_clean

    ; reload regs->eax
    mov ebx, [eax + 52]

    ; load syscall address
    push eax            ; save *regs

    mov eax, syscalls   ; *syscalls
    imul ebx, 4         ; regs->eax * 4 (pointer size)
    add eax, ebx        ; (syscalls + 4 * n)

    mov ebx, [eax]      ; *(syscalls + 4 *n)
    pop eax             ; load *regs

    ; call function
    push word [eax + 28] ; arguments (esi)
    push word [eax + 24] ; (edi)
    push word [eax + 44] ; (edx)
    push word [eax + 48] ; (ecx)
    push word [eax + 40] ; (ebx)

    call ebx

    pop word eax
    pop word eax
    pop word eax
    pop word eax
    pop word eax

    ; clean
    jmp syscall_clean

    ret

syscall_clean:
    ; restore registers
    push edx
    pop ecx
    pop ebx
    pop eax

    ret
