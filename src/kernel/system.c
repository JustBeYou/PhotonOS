/* Basic System Interface
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

#include <i386/pit.h>
#include <kernel/io.h>

void panic(const char *msg, int line, char *file)
{
	cli();
	printk("[KERNEL PANIC]");
	printk("%s. Error at line %d in file %s.", msg, line, file);
	keep_running();
}

void reboot()
{
	uint8_t good = 0x02;
	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	hlt();
}

void system_wait(int ticks)
{
    volatile int start_tick = get_tick();
    while (get_tick() - start_tick >= ticks) {
        ;
    }
}
