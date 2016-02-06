GLOBAL jmp_to_usermode ;you may need to remove this _ to work right..
GLOBAL test_user

test_user:
    int 0x5

jmp_to_usermode:
    mov ax,0x23
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax ;we don't need to worry about SS. it's handled by iret

    mov eax,esp
    push 0x23 ;user data segment with bottom 2 bits set for ring 3
    push eax ;push our current stack just for the heck of it
    pushf
    push 0x1B; ;user code segment with bottom 2 bits set for ring 3
    push test_user ;may need to remove the _ for this to work right
    iret
