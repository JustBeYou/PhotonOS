/* Interrupt service routines & Interrupt requests
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This file contains declarations of software interrupts.
 * There are 32 declarations (from 0 to 31) plus a special interrupt (128 or 0x80)
 * that is used to call kernel when user try need system calls.
 * Declarations of the 16 interrupts requests and of their general handler.
 * Interrupt request are generated only by hardware.
 */

#ifndef _handlers_h
#define _handlers_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

/**
  * Initialize the ISR.
  */
void init_isr();

/**
  * Handle the isr.
  */
void isr_handler(registers_t regs);

// isr handlers
extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr128();

#define IRQ0 32 // Programmable Interrupt Timer Interrupt (handled)
#define IRQ1 33 // Keyboard Interrupt (handled)
#define IRQ2 34 // Cascade (used internally by the two PICs. never raised)
#define IRQ3 35 // COM2 (if enabled)
#define IRQ4 36 // COM1 (if enabled)
#define IRQ5 37 // LPT2 (if enabled)
#define IRQ6 38 // Floppy Disk (handled)
#define IRQ7 39 // LPT1 / Unreliable "spurious" interrupt (usually)
#define IRQ8 40 // CMOS real-time clock (if enabled)
#define IRQ9 41 // Free for peripherals / legacy SCSI / NIC
#define IRQ10 42 // Free for peripherals / SCSI / NIC
#define IRQ11 43 // Free for peripherals / SCSI / NIC
#define IRQ12 44 // PS2 Mouse
#define IRQ13 45 // FPU / Coprocessor / Inter-processor
#define IRQ14 46 // Primary ATA Hard Disk
#define IRQ15 47 // Secondary ATA Hard Disk

/**
  * Initialize the IRQ.
  */
void init_irq();

/**
  * Handle the irq.
  */
void irq_handler(registers_t regs);

// irq handlers
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif
