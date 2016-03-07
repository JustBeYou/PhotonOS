#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>
#include <kernel/io.h>

void *syscalls[] = {
	&vga_write,
    &kb_read
};
int num_syscalls = 2;

void syscall_handler(registers_t *regs)
{
	/*if (regs->eax > (uint32_t) num_syscalls)
		return;

	void *location = syscalls[regs->eax - 1];

    int ret;*/
    // TODO: rewrite this function in asm
}
