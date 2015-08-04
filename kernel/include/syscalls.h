#ifndef _syscalls_h
#define _syscalls_h

#include <system.h>
#include <io.h>

void syscall_handler(registers_t *regs);
void init_syscalls();

#endif
