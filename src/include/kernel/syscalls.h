#ifndef _syscalls_h
#define _syscalls_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>

/**
  * System calls handler.
  */
void syscall_handler(registers_t *regs);

#endif
