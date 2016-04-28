#ifndef _syscalls_h
#define _syscalls_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>

extern uint32_t syscall(uint32_t eax, uint32_t ebx, uint32_t ecx,
                        uint32_t edx, uint32_t esi, uint32_t edi);
                        
#endif
