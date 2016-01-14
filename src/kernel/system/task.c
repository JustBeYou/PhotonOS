/* Tasking interface
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

/*** REMOVED BECAUSE IS UNSTABLE ***/
/*** TODO: GET STABLE TASKING ***/

task_t *current_task;
task_t *start_task;
uint32_t pid;
int start_tasking = 0;

void *syscalls[] = {
	&write
};
int num_syscalls = 1;

extern void switch_task(t_regs_t *from, t_regs_t *to);
extern void kernel_main();

void init_usermode() {
	set_kernel_stack((uint32_t) stack_top);
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

void syscall_handler(registers_t *regs)
{
	if (regs->eax > (uint32_t) num_syscalls)
		return;

	void *location = syscalls[regs->eax - 1];

	int ret;
	asm volatile (" \
		push %1; \
		push %2; \
		push %3; \
		push %4; \
		push %5; \
		call *%6; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
		" : "=a" (ret) : "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (location));
		regs->eax = ret;
}

void init_tasking()
{
	pid = 1;
	uint32_t cr3, eflags;
	asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=r"(cr3)::"%eax");
	asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=r"(eflags)::"%eax");
	
	start_task = create_task(start_task, kernel_main, eflags, cr3);
	
	current_task = start_task;
}

task_t *create_task(task_t *new_task, void (*main)(), int32_t flags, uint32_t pagedir)
{
	new_task = (task_t*) kmalloc(sizeof(task_t), 0, 0);
	printk("%x\n", new_task);
	new_task->pid = pid;
	new_task->regs.eax = 0;
	new_task->regs.ebx = 0;
	new_task->regs.ecx = 0;
	new_task->regs.edx = 0;
	new_task->regs.esi = 0;
	new_task->regs.edi = 0;
	new_task->regs.eflags = flags;
	new_task->regs.eip = (uint32_t) main;
	new_task->regs.cr3 = (uint32_t) pagedir;
	new_task->regs.esp = (uint32_t) kmalloc(2000, 0, 0);
	new_task->next = 0;
	
	if (pid != (uint32_t) 1) {
		task_t *tmp_task = start_task;
		while (tmp_task->next)
			tmp_task = tmp_task->next;
		tmp_task->next = new_task;
	}
	pid++;
	return new_task;
}

void preempt()
{
	cli();
	printk("Current task: %x\n", current_task);
	printk("Start task: %x\n", start_task);
	printk("Main address: %x\n", current_task->regs.eip);
	if (current_task->next != NULL) {
	    switch_task(&(current_task->regs), &(current_task->next->regs));
	    keep_running();
	    current_task = current_task->next;
	} else {
	    switch_task(&(current_task->regs), &(start_task->regs));
	    keep_running();
	    current_task = start_task;
	}
	
	sti();
}
