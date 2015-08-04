/* Global descriptor table
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * The Global Descriptor Table or GDT is a data structure used by Intel
 * x86-family processors starting with the 80286 in order to define
 * the characteristics of the various memory areas used during program
 * execution, including the base address, the size and access privileges
 * like executability and writability. These memory areas are called
 * segments in Intel terminology.
 */

#ifndef _gdt_h
#define _gdt_h

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <system.h>

/* The GDT */

/**
  * The struct of a GDT entry.
  */
struct gdt_entry_struct
{
	uint16_t lim_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

/**
  * The struct of a GDT pointer.
  */
struct gdt_ptr_struct
{
	uint16_t lim;
	uint32_t base;
} __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

/**
  * The struct of a TSS.
  */
struct tss_entry_struct
{
	uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
	uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
	uint32_t ss0;        // The stack segment to load when we change to kernel mode.
	uint32_t esp1;       // Unused...
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;         // The value to load into ES when we change to kernel mode.
	uint32_t cs;         // The value to load into CS when we change to kernel mode.
	uint32_t ss;         // The value to load into SS when we change to kernel mode.
	uint32_t ds;         // The value to load into DS when we change to kernel mode.
	uint32_t fs;         // The value to load into FS when we change to kernel mode.
	uint32_t gs;         // The value to load into GS when we change to kernel mode.
	uint32_t ldt;        // Unused...
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed));
typedef struct tss_entry_struct tss_entry_t; 

tss_entry_t tss_entry;

/**
  * Flush the TSS. 
  */
extern void tss_flush();

/**
  * Write the TSS. 
  */
void write_tss(int32_t num, uint16_t ss0, uint32_t esp0);

/**
  * Set current proccess stack.
  */
void set_kernel_stack(uint32_t stack);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;

/**
  * Flush the GDT. Extern function implemented in 'asm_dt.asm'.
  */
extern void gdt_flush(uint32_t);

/**
  * Initialize the GDT.
  */
void init_gdt();

/**
  * Set parametters of a GDT gate.
  */
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

#endif
