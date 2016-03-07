/* Time interface
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

#include <kernel/io.h>
#include <kernel/time.h>

int mseconds;
int seconds;
int minutes;
int hours;

void init_time()
{
	mseconds = 0;
	seconds = 0;
	minutes = 0;
	hours = 0;
}

void update_time()
{
	mseconds++;
	if (mseconds == 100) {
		seconds++;
		mseconds = 0;
	}
	if (seconds == 60) {
		minutes++;
		seconds = 0;
	}
	if (minutes == 60) {
		hours++;
		minutes = 0;
		seconds = 0;
	}
	if (hours == 24) {
		hours = 0;
	}
}

void print_time()
{
	printk("Time: %d:%d:%d:%d ", hours, minutes, seconds, mseconds);
	if (hours < 12) {
		printk("am");
	} else {
		printk("pm");
	}
	printk("\n");
}
