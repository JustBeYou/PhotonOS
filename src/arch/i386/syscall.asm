extern num_syscalls
extern syscalls

[global syscall]
syscall:
    push ebp
    mov ebp, esp

	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi

    mov eax, [ebp + 8]
    mov ebx, [ebp + 12]
    mov ecx, [ebp + 16]
    mov edx, [ebp + 20]
    mov esi, [ebp + 24]
    mov edi, [ebp + 28]
    
    int 0x80

	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
    
    pop ebp
    ret

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
    push eax            ; save *regs
    
    mov ecx, [eax + 28] ; arguments (esi)
    push ecx

    mov ecx, [eax + 24] ; (edi)
    push ecx

    mov ecx, [eax + 44] ; (edx)
    push ecx

    mov ecx, [eax + 48] ; (ecx)
    push ecx

    mov ecx, [eax + 40] ; (ebx)
    push ecx

    call ebx

    add esp, 20
    pop ebx             ; load *regs
    mov [ebx + 52], eax ; store return value in regs->eax

    ; clean
    jmp syscall_clean

    ret

syscall_clean:
    ; restore registers
    pop edx
    pop ecx
    pop ebx
    pop eax

    ret
