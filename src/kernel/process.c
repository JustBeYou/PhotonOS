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

#include <i386/gdt.h>
#include <kernel/heap.h>
#include <kernel/process.h>


/*** REMOVED BECAUSE IS UNSTABLE ***/
/*** TODO: GET STABLE processING ***/

extern int time_to_run;
extern uint32_t user_stack;
process_t *current_process;
process_t *start_process;
uint32_t pid;
int switch_on = 0;

void init_multitasking()
{
    uint32_t cr3 = read_cr3();

	pid = 2;

    start_process = (process_t*) kmalloc(sizeof(process_t), 0, 0);
    start_process->pid = 1;
    start_process->time_to_run = 100;
    start_process->ready = 1;
    start_process->prior = 0;
    memcpy(start_process->name, "kernel\0", 7);
    start_process->next = NULL;

    start_process->esp = 0;
    start_process->ss = 0x13;
    start_process->kern_stack = kmalloc(PROC_KERN_STACK, 1, 0) + PROC_KERN_STACK;
    start_process->user_stack = kmalloc(PROC_USER_STACK, 1, 0) + PROC_USER_STACK;
    user_stack = start_process->user_stack;
    set_kernel_stack(start_process->kern_stack);
    start_process->cr3 = 0;

    time_to_run = 0;

	current_process = start_process;
}

/* Useless until there is a file system. */
process_t *create_process(char *name)
{
    uint32_t cr3 = read_cr3();

	process_t *new_process = (process_t*) kmalloc(sizeof(process_t), 0, 0);
	new_process->pid = pid;
    new_process->time_to_run = 10;
    new_process->ready = 1;
    new_process->prior = 0;
    memcpy(new_process->name, name, strlen(name) + 1);
	new_process->next = NULL;

    /* Here I should load the file and create a context for the new process. */

	if (pid != (uint32_t) 1) {
		process_t *tmp_process = start_process;
		while (tmp_process->next)
			tmp_process = tmp_process->next;
		tmp_process->next = new_process;
	}
	pid++;
	return new_process;
}
