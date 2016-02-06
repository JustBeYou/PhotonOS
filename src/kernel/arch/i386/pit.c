/* Programmable interval timer driver
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

extern mem_heap_t *kernel_heap;
extern int tick;
extern volatile int in_cursor;
int time_to_run = 0;

void timer_callback(__attribute__ ((unused)) registers_t *regs)
{
    cli();
    tick++;
    update_time();
    if (in_cursor >= STDIO_SIZE) {
        in_cursor = 0;
    }

    if (--time_to_run <= 0) {
        printk("Process ran out of time. Switching...\n");
        switch_process();
    }
    sti();
}

void init_timer(uint32_t frequency)
{
   init_time();
   tick = 0;
   // Firstly, register our timer callback.
   register_interrupt_handler(IRQ0, &timer_callback);

   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   uint32_t divisor = 1193180 / frequency;

   // Send the command byte.
   outb(0x43, 0x36);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)( (divisor >> 8) & 0xFF );

   // Send the frequency divisor.
   outb(0x40, l);
   outb(0x40, h);
}

int get_tick()
{
    return tick;
}
