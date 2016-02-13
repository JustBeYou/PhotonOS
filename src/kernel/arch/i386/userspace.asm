extern kernel_main
extern user_stack

[global jmp_to_usermode]
jmp_to_usermode:
    cli

    mov esp, [user_stack]

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

    lea eax, [kernel_main]
    push eax

    iret

extern current_process
extern start_process
extern tss_entry
extern time_to_run

[global save_context]
save_context:
    lea eax, [esp] ; get the kernel stack position to save
    mov ebx, [current_process] ; get the address of *current_process

    mov [ebx+4], eax ; save esp into current_process
    mov [ebx+8], ss ; save ss into current_process

    ret

[global switch_context]
switch_context:
    mov eax, [current_process] ; get the address of *current_process

    mov esp, [eax+4] ; load new esp
    mov ss, [eax+8] ; load new ss

    lea eax, [esp]
    mov [tss_entry+4], eax ; load new esp to tss_entry
    mov [tss_entry+8], ss ; load new ss to tss_entry

    ret

[global switch_mm]
switch_mm:
    ret ; to be implemented!

[global switch_to]
switch_to:
    call save_context

    ; get current_process->next and compare with NULL
    mov eax, [current_process]
    mov ebx, [eax+68]
    cmp ebx, 0
    je proc_next_null

    ; current_process = current_process->next
    mov ebx, [eax+68]
    lea eax, [current_process]
    mov [eax], ebx

switch_context_label:
    ;call switch_mm
    call switch_context

    ret

proc_next_null:
    lea eax, [current_process]
    lea ebx, [start_process]
    mov ecx, [ebx]
    mov [eax], ecx

    jmp switch_context_label
