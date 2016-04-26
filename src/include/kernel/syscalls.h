#ifndef _syscalls_h
#define _syscalls_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>

#define syscall(eax, ebx, ecx, edx, esi, edi) \
        asm volatile ("movl %0, %%eax;" \
        "movl %1, %%ebx;" \
        "movl %2, %%ecx;" \
        "movl %3, %%edx;" \
        "movl %4, %%esi;" \
        "movl %5, %%edi;" \
        "int $0x80;" \
        : : "r" (eax) , "r" (ebx) , "r" (ecx) , "r" (edx) , "r" (esi) , "r" (edi))

#endif
