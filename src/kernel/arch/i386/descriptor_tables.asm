; Descriptor tables
 ;
 ; Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 ; This program is free software; you can redistribute it and/or modify it
 ; under the terms of the GNU General Public License as published by the Free
 ; Software Foundation; either version 2 of the License, or (at your option)
 ; any later version.
 ;
 ; Flushing functions for GDT & IDT & TSS
 ;

[GLOBAL gdt_flush]    ; Allows the C code to call gdt_flush().

gdt_flush:
   mov eax, [esp+4]  ; Get the pointer to the GDT, passed as a parameter.
   lgdt [eax]        ; Load the new GDT pointer

   mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
   mov ds, ax        ; Load all data segment selectors
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   jmp 0x08:.flush   ; 0x08 is the offset to our code segment: Far jump!
.flush:
   ret
   
[GLOBAL idt_flush]    ; Allows the C code to call idt_flush().

idt_flush:
   mov eax, [esp+4]  ; Get the pointer to the IDT, passed as a parameter.
   lidt [eax]        ; Load the IDT pointer.
   ret
   
[GLOBAL tss_flush]
tss_flush:
    mov ax, 0x2B
    ltr ax
    ret
    
[GLOBAL read_eip]

read_eip:
    pop eax
    jmp eax
