#include <syscalls.h>
#include <io.h>
#include <isr.h>

void *syscalls[5] = {
	NULL,
	NULL, 
	NULL,
	&write,
	NULL
};
unsigned int num_syscalls = 5;

void syscall_handler(registers_t *regs)
{
	if (regs->eax >= num_syscalls || regs->eax < 1)
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

void init_syscalls()
{
	register_interrupt_handler(0x80, &syscall_handler);
}
