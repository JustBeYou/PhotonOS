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
#include <init.h>
#include <multiboot.h>
#include <task.h>

void usermode() {
    asm volatile("  \
      cli; \
      mov $0x23, %ax; \
      mov %ax, %ds; \
      mov %ax, %es; \
      mov %ax, %fs; \
      mov %ax, %gs; \
                    \
       \
      mov %esp, %eax; \
      pushl $0x23; \
      pushl %esp; \
      pushf; \
      pushl $0x1B; \
      push $1f; \
      iret; \
    1: \
      ");
}

void kmain(multiboot *mboot_ptr) {
	init();
	printk("\n\nMultiboot size: %d\n\n", mboot_ptr->size);
	getch();
	welcome();
	login();
	usermode();
	
	while(true) {
	
	} 
	
	prompt();
	panic("Shell exited!", __LINE__, __FILE__);
}
