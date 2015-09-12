/* Basic System Interface
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <system.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pit.h>
#include <kheap.h>
#include <vmm.h>
#include <pmm.h>
#include <io.h>
#include <vga.h>

uint32_t init_esp;

char user[20];
char machine[30];

static void testing_shell(char *str) 
{

	while (true) {
		printk("\nEnter test name> ");
		gets(str);
		
		if (!strcmp(str, "exit")) {
			break;
		} else if (!strcmp(str, "print-stdin")) {
			printk("%s\n", (uint8_t*)stdin);
		} else if (!strcmp(str, "size")) {
			printk("Sizes:\n");
			printk("Char: %d\nInt: %d\nFloat: %d\nSize_t: %d\n", sizeof(char), sizeof(int), sizeof(float), sizeof(size_t));
			printk("Kernel:\n");
			printk("Start: %x\nEnd: %x\n", (size_t)&kernel_start, (size_t)&kernel_end);
			//printk("RAM: %d MB\n", mem_size_mb);
		} else if (!strcmp(str, "page-fault")) {
			printk("Test fault...\n");
			uint32_t *ptr = (uint32_t*)0xA123142300;
			*ptr = 432;
			printk("%d\n", *ptr);
		} else if (!strcmp(str, "address")) {
			printk("This is a test of memory manager. Warning! You are in kernel mode!\n");
			printk("Kernel:\n");
			printk("Start: %x\nEnd: %x\n", (size_t)&kernel_start, (size_t)&kernel_end);
			getch();
			printk("stack_top: %x\n", stack_top);
			printk("&stack_top: %x\n", &stack_top);
			getch();
			
			int *num = (int*) kmalloc(sizeof(int), 0, 0);
			*num = 0x123ABC;
			printk("num: %x\n", num);
			printk("&num: %x\n", &num);
			printk("*num: %x\n", *num);
			
			int snum = 0xABC123;
			printk("snum: %x\n", snum);
			printk("&snum: %x\n", &snum);
			getch();
			
			printk("Kernel:\n");
			printk("Start: %x\nEnd: %x\n", (size_t)&kernel_start, (size_t)&kernel_end);
			getch();
			printk("stack_top: %x\n", stack_top);
			printk("&stack_top: %x\n", &stack_top);
			getch();
			
			kfree(num);
		} else if (!strcmp(str, "alloc")) {
			printk("SIZE OF INT: %d\n", sizeof(int));
			int *a = (int *)kmalloc(sizeof(int), 0, 0);
			*a = 6;
			printk("a: %x\n", a);
			printk("a: %x\n", *a);
			
			int *b = (int *)kmalloc(sizeof(int), 0, 0);
			*b = 5;
			printk("b: %x\n", b);
			printk("b: %x\n", *b);
			kfree(b);
			
			int *c = (int *)kmalloc(sizeof(int), 0, 0);
			*c = 8;
			printk("c: %x\n", c);
			printk("c: %x\n", *c);
			
			kfree(a);
			kfree(b);
			kfree(c);
			//-------------------------------------------------------
			char *stmp = (char *)kmalloc(sizeof(char) * 11, 0, 0);
			memcpy(stmp, "This is.\0", 9);
			printk(stmp);
			printk("\n&stmp[0]: %x &stmp[9]: %x\n", stmp, stmp + sizeof(char) * 11);
			
			kfree(stmp);
			
			char *tmp = (char *)kmalloc(sizeof(char) * 10, 0, 0);
			memcpy(tmp, "THIS WAS.\0", 10);
			printk(tmp);
			printk("\n&tmp[0]: %x &tmp[9]: %x\n", tmp, tmp + sizeof(char) * 11);
			
			kfree(tmp);
		} else if (!strcmp(str, "vga")) {
			printk("Ab_C\b\b\nNew\tLineWasWritten NUM~%x%s%c%c\n", 0x123ABC, "test", 'f', 55);
			printk("Expected:\nAb\nNew    LineWasWritten NUM~0x123abctestf7\n");
		} else {
			printk("Command not found!\n");
		}
	}
}

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

void shell(char *str) {
	if (!strcmp(str, "exit")) {
			panic("Exit.", __LINE__, __FILE__);
	} else if (!strcmp(str, "test")) {
		testing_shell(str);
		
	} else if (!strcmp(str, "time")) {
		print_time();
		printk("\n");
		
	} else if (!strcmp(str, "tick")) {
		printk("Tick: %d\n", get_tick());
	
	} else if (!strcmp(str, "clock")) {
		printk("WARN: no clock yet\n");
	} else if (!strcmp(str, "clear")) {
		clear_vga();
		
	} else if (!strcmp(str, "reset-clock")) {
		hours = 0;
		minutes = 0;
		seconds = 0;
		
	} else if (!strcmp(str, "write")) {
		char *to_write = (char*) kmalloc(sizeof(char) * 30, 0, 0);
		size_t len = strlen("\nLet's write this...\n");
		
		memcpy(to_write, "Let's write this...\n\0", len + 1);
		call(1, (uint32_t) to_write, (uint32_t) len, 0, 0, 0);
		kfree(to_write);
		
	} else if (!strcmp(str, "reboot")) {
		reboot();
		
	} else if (!strcmp(str, "huge-alloc")) {
		int *i = (void *) kmalloc(sizeof(int), 0, 0);
		kfree(i);
	} else if (!strcmp(str, "usermode")) {
		printk("CPU will jump to usermode, this will stop shell, because it is running only in kernel mode.\n");
		init_usermode();
		char s[] = "Welcome in usermode!\n";
		call(1, (uint32_t) s, (uint32_t) strlen(s), 0, 0, 0);
	} else if (!strcmp(str, "sys-info")) {
		printk("%s %s (%s) by %s.\nCopyright C 2015 %s.\nAll rights reserved.\n", OS_Name, Version, Relase_Date, Author, Author);
		
	} else if (!strcmp(str, "help")) {
		printk("After kernel boots up, user must enter user name and machine name. Now, a shell has started. Here user can use some commands:\n");
		printk("	-> help -> display this page\n");
		printk("	-> time -> display current time\n");
		printk("	-> tick -> display current tick\n");
		printk("	-> clock -> start a clock that can be stoped when ESC is pressed\n");
		printk("	-> clear -> clear the screen\n");
		printk("	-> reset-clock -> set clock to 0\n");
		printk("	-> sys->info -> prints info about system\n");
		printk("	-> test -> enter into testing shell that allows testing kernel functions\n");
		printk("	-> exit -> close shell\n\n");
	
		printk("Testing shell:\n");
		printk("	Tests avabile in testing shell:\n");
		printk("		-> exit -> close testing shell\n");
		printk("		-> print-stdin -> print all data from standard input\n");
		printk("		-> size -> print sizes of types, of kernel and RAM\n");
		printk("		-> page-fault -> generate page fault\n");
		printk("		-> address -> print addresses of all kernel variables and of some temporary\n");
		printk("		-> alloc -> test kmalloc and kfree\n");
		
	} else {
		printk("\"%s\" not recognized as internal command or program. Please try again.\n", str);
	}
}

void prompt() {
	char cmd[1024];
	while (true) {
		printk("%s@%s:$ ", user, machine);
		gets(cmd);
		shell(cmd);
	}
}
