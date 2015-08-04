#include <time.h>
#include <pit.h>
#include <stdio.h>
#include <io.h>

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
}
