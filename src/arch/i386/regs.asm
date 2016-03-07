[global flush_tlb]
flush_tlb:
    push ebx
    mov cr3, ebx
    mov ebx, cr3
    pop ebx

[global read_cr0]
read_cr0:
    mov eax, cr0
    retn

[global read_cr2]
read_cr2:
    mov eax, cr2
    retn

[global write_cr0]
write_cr0:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    mov cr0,  eax
    pop ebp
    retn

[global read_cr3]
read_cr3:
    mov eax, cr3
    retn

[global write_cr3]
write_cr3:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    mov cr3, eax
    pop ebp
    retn

[global read_eflags]
read_eflags:
    pushf
    pop eax
    retn

[global read_ss]
read_ss:
    mov eax, ss
    retn

[global read_cs]
read_cs:
    mov eax, cs
    retn

[global read_ds]
read_ds:
    mov eax, ds
    retn


[global read_ebp]
read_ebp:
    mov eax, ebp
    retn

[global read_esp]
read_esp:
    lea eax, [esp+4]
    retn
