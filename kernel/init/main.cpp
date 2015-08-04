/* All starts here!
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <kernel_class.h>

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gdt.h>
#include <idt.h>
#include <pit.h>
#include <vga.h>
#include <handlers.h>
#include <io.h>
#include <system.h>
#include <keyboard.h>
#include <time.h>
#include <kheap.h>
#include <paging.h>
#include <init.h>
#include <multiboot.h>
#include <task.h>

void kmain(multiboot *mboot_ptr, uint32_t init_stack) {
	init(mboot_ptr, init_stack);
	welcome();
	login();
	
	/* C++ Test */
	printk("\n");
	
 	KernelClass my_kernel;
	my_kernel.setID(16072015);
	my_kernel.setVersion("v0.0.1cpp");
	int ID = my_kernel.getID();
	char version[30];
	memcpy(version, my_kernel.getVersion(), strlen(my_kernel.getVersion()) + 1);
	
	printk("KERNEL ID: %d\n", ID);
	printk("KERNEL VERSION: %s\n", version);
	
	printk("\n");
	
	prompt();
	panic("Shell exited!", __LINE__, __FILE__);
}

#ifdef __cplusplus 
}
#endif
