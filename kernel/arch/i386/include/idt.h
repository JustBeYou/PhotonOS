/* Interrupt descriptor table
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * The Interrupt Descriptor Table (IDT) is a data structure used by the x86
 * architecture to implement an interrupt vector table. The IDT is used by
 * the processor to determine the correct response to interrupts and exceptions.
 */

#ifndef _idt_h
#define _idt_h

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <system.h>

/* The IDT */

/**
  * The struct of a IDT entry.
  */
struct idt_entry_struct
{
    uint16_t base_lo;
    uint16_t sel;               
    uint8_t  always0;             
    uint8_t  flags;              
    uint16_t base_hi;             
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

/**
  * The struct of a IDT pointer.
  */
struct idt_ptr_struct
{
    uint16_t limit;
    uint32_t base;              
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr; 

/**
  * Flush the IDT. Extern function implemented in 'asm_dt.asm'.
  */
extern void idt_flush(uint32_t);

/**
  * Initialize the IDT.
  */
void init_idt();

/**
  * Set parameters of IDT gate.
  */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
