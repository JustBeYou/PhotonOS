[GLOBAL init_usermode]

init_usermode:
	mov ax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov eax, esp
	push 0x23
	push eax
	pushf
	
	pop eax
	or eax, 0x200
	push eax
	
	push 0x1B
	iret
