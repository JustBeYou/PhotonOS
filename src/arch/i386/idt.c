/* Interrupt descriptor tables
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/gdt.h>
#include <i386/idt.h>
#include <i386/handlers.h>
#include <kernel/io.h>

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

static void unhandled_interrupt(__attribute__((unused)) registers_t *regs)
{
    print_regs(regs);
    panic("Interrupt unhandled!", __LINE__, __FILE__);
}

void init_idt()
{
    idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

    for (int n = 0; n < 256; n++) {
        register_interrupt_handler(n, &unhandled_interrupt);
    }

    init_isr();
    init_irq();

    idt_flush((uint32_t)&idt_ptr);
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags | 0x60;
}
