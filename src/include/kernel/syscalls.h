#ifndef _syscalls_h
#define _syscalls_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>

extern size_t syscall(size_t eax, size_t ebx, size_t ecx,
                        size_t edx, size_t esi, size_t edi);
                        
#endif
