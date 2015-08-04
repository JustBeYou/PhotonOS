#include <system.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pit.h>
#include <tty.h>
#include <kheap.h>
#include <paging.h>
#include <io.h>

void panic(char *msg, int line, char *file)
{
	printk("[KERNEL PANIC]");
	printk("%s. Error at line %d in file %s.", msg, line, file);
	cli();
	keep_running();
}

void exec(char *str) {
	if (!strcmp(str, "exit")) {
		panic("Shell exited!", __LINE__, __FILE__);
		
	} else if (!strcmp(str, "time")) {
		print_time();
		printk("\n");
		
	} else if (!strcmp(str, "tick")) {
		printk("Tick: %d\n", get_tick());
		
	} else if (!strcmp(str, "clock")) {
		int lasttick = get_tick();
		while (true) {
			if (get_tick() > lasttick) {
				clear_line(tty_row);
				print_time();
			}
			if (((uint8_t*)stdin)[in_size] == 27) {
				printk("\n");
				((uint8_t*)stdin)[in_size] = 0;
				break;
			}
			lasttick = get_tick();
		}
		
	} else if (!strcmp(str, "clear")) {
		tty_clear();
	} else if (!strcmp(str, "reset-clock")) {
		hours = 0;
		minutes = 0;
		seconds = 0;
	} else if (!strcmp(str, "print-stdin")) {
		printk("%s\n", (uint8_t*)stdin);
	} else if (!strcmp(str, "size")) {
		printk("Sizes:\n");
		printk("Char: %d\nInt: %d\nFloat: %d\nSize_t: %d\n", sizeof(char), sizeof(int), sizeof(float), sizeof(size_t));
		printk("Kernel:\n");
		printk("Start: %x\nEnd: %x\n", (size_t)&kernel_start, (size_t)&kernel_end);
	} else if (!strcmp(str, "page-fault")) {
		printk("Test fault...\n");
		uint32_t *ptr = (uint32_t*)0xA123142300;
		printk("%d\n", *ptr);
	} else if (!strcmp(str, "sys-info")) {
		printk("%s %s (%s) by %s. All rights reserved.\n", OS_Name, Version, Relase_Date, Author);
	} else {
		printk("Command not found!\n");
	}
}

void prompt() {
	char cmd[1024];
	while (true) {
		printk("%s@%s:$ ", user, machine);
		gets(cmd);
		exec(cmd);
	}
}
