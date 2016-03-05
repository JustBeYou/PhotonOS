#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

void *syscalls[] = {
	&vga_write,
    &kb_read
};
int num_syscalls = 2;

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
