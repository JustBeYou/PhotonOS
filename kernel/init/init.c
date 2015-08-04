#include <tty.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gdt.h>
#include <idt.h>
#include <pit.h>
#include <vga.h>
#include <isr.h>
#include <io.h>
#include <system.h>
#include <keyboard.h>
#include <time.h>
#include <kheap.h>
#include <paging.h>
#include <stddef.h>
#include <task.h>
#include <syscalls.h>

#define FREQ 100
uint8_t inbuffer[STDIN_SIZE];

void init_stdio() {
	stdin = (uint8_t*) inbuffer;
	for (int i = 0; i <= STDIN_SIZE; i++) {
		((uint8_t*)stdin)[i] = 0;
	}
} 

void init() {
	tty_init();
	printk("Initialized tty.\n");

	init_gdt();
	printk("Initialized GDT.\n");
	
	init_idt();
	printk("Initialized IDT and interrupts.\n");
	
	init_timer(FREQ);
	printk("Install timer and clock.\n");
	
	init_stdio();
	printk("Initialized stdio.\n");
	
	install_keyboard();
	printk("Install keyboard support.\n");
	
	printk("Kernel starts at: %x\n", (size_t)&kernel_start);
	printk("Kernel ends at: %x\n", (size_t)&kernel_end);
	placement_address = (size_t)&kernel_end;
	kheap = NULL;
	init_paging();
	printk("Paging initialized.\n");
	
	init_syscalls();

	printk("Done!");
	sti();
	
	getch();
}

void welcome() {
	tty_clear();	
	
	wstr_color("      |\\     /|(  ____ \\( \\      (  ____ \\(  ___  )(       )(  ____ \\\n", COLOR_RED);
	wstr_color("      | )   ( || (    \\/| (      | (    \\/| (   ) || () () || (    \\/\n", COLOR_RED);
	wstr_color("      | | _ | || (__    | |      | |      | |   | || || || || (\\\n", COLOR_RED);
	wstr_color("      | |( )| ||  __)   | |      | |      | |   | || |(_)| ||  __)\\\n", COLOR_RED);   
	wstr_color("      | || || || (      | |      | |      | |   | || |   | || (\\\n", COLOR_RED);      
	wstr_color("      | () () || (____/\\| (____/\\| (____/\\| (___) || )   ( || (____/\\\n", COLOR_RED);
	wstr_color("      (_______)(_______/(_______/(_______/(_______)|/     \\|(_______/\n\n", COLOR_RED);
	
	wstr_color("                           \\__   __/(  ___  )\n", COLOR_YELLOW);
	wstr_color("                              ) (   | (   ) |\n", COLOR_YELLOW);
	wstr_color("                              | |   | |   | |\n", COLOR_YELLOW);
	wstr_color("                              | |   | |   | |\n", COLOR_YELLOW);
	wstr_color("                              | |   | |   | |\n", COLOR_YELLOW);
	wstr_color("                              | |   | (___) |\n", COLOR_YELLOW);
	wstr_color("                              )_(   (_______)\n\n", COLOR_YELLOW);
	
	wstr_color("      ( (    /|(  ____ \\|\\     /|(  ___  )(  ____ \\( )\n", COLOR_BLUE);
	wstr_color("      |  \\  ( || (    \\/| )   ( || (   ) || (    \\/| |\n", COLOR_BLUE);
	wstr_color("      |   \\ | || (__    | | _ | || |   | || (_____ | |\n", COLOR_BLUE);
	wstr_color("      | (\\ \\) ||  __)   | |( )| || |   | |(_____  )| |\n", COLOR_BLUE);
	wstr_color("      | | \\   || (      | || || || |   | |      ) |(_)\n", COLOR_BLUE);
	wstr_color("      | )  \\  || (____/\\| () () || (___) |/\\____) | _ \n", COLOR_BLUE);
	wstr_color("      |/    )_)(_______/(_______)(_______)\\_______)(_)\n\n", COLOR_BLUE);
	
	printk("                                                        by Feraru Mihail");
	
	getch();
	tty_clear();
}

void login() {
	wstr_color("      ( (    /|(  ____ \\|\\     /|(  ___  )(  ____ \\( )\n", COLOR_BLUE);
	wstr_color("      |  \\  ( || (    \\/| )   ( || (   ) || (    \\/| |\n", COLOR_BLUE);
	wstr_color("      |   \\ | || (__    | | _ | || |   | || (_____ | |\n", COLOR_BLUE);
	wstr_color("      | (\\ \\) ||  __)   | |( )| || |   | |(_____  )| |\n", COLOR_BLUE);
	wstr_color("      | | \\   || (      | || || || |   | |      ) |(_)\n", COLOR_BLUE);
	wstr_color("      | )  \\  || (____/\\| () () || (___) |/\\____) | _ \n", COLOR_BLUE);
	wstr_color("      |/    )_)(_______/(_______)(_______)\\_______)(_)\n\n", COLOR_BLUE);
	
	printk("Log in please.\n");
	printk("Username: ");
	gets(user);
	printk("Machine: ");
	gets(machine);
	printk("Loged in!\n");
}
