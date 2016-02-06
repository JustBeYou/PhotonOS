/* processing interface
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
/*** TODO: GET STABLE processING ***/

extern int time_to_run;
process_t *current_process;
process_t *start_process;
uint32_t pid;
int start_processing = 0;

void *syscalls[] = {
	&write
};
int num_syscalls = 1;

extern void kernel_main();
extern void jmp_to_usermode();

void init_usermode() {
	set_kernel_stack((uint32_t) stack_top);
    jmp_to_usermode();
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

void init_multitasking()
{
    uint32_t cr3;

    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=r"(cr3)::"%eax");
	pid = 2;

    start_process = (process_t*) kmalloc(sizeof(process_t), 0, 0);
    start_process->pid = 1;
    start_process->cr3 = cr3;
    start_process->esp = stack_top;
    start_process->time_to_run = 100;
    start_process->prior = 0;
    memcpy(start_process->name, "kernel", 7);
    start_process->next = NULL;
    time_to_run = 10;

	current_process = start_process;
    printk("cr3: %x esp: %x\n", current_process->cr3, current_process->esp);
}

process_t *create_process(char *name)
{
    uint32_t cr3;
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=r"(cr3)::"%eax");

	process_t *new_process = (process_t*) kmalloc(sizeof(process_t), 0, 0);
	new_process->pid = pid;
	new_process->cr3 = cr3;
	new_process->esp = (uint32_t) kmalloc(2000, 1, 0);
    new_process->time_to_run = 10;
    new_process->prior = 0;
    memcpy(new_process->name, name, strlen(name) + 1);
	new_process->next = NULL;

	if (pid != (uint32_t) 1) {
		process_t *tmp_process = start_process;
		while (tmp_process->next)
			tmp_process = tmp_process->next;
		tmp_process->next = new_process;
	}
	pid++;
	return new_process;
}

void switch_process()
{
    if (current_process->next != NULL) {

    } else {
        printk("Go to start proc.\n");
        printk("cr3: %x esp: %x\n", current_process->cr3, current_process->esp);
        current_process = start_process;
        write_cr3(current_process->cr3);
        asm volatile ( " \
            mov %%esp, %0 \
            " : "=a" (current_process->esp)
        );
    }
    time_to_run = current_process->time_to_run;
}
