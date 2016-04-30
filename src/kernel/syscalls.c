#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>
#include <kernel/io.h>

void useless(void)
{
    return ;
}

int not_implemented_syscall(void)
{
    return 0xDEADBEEF;
}

void *syscalls[] = {
    &useless,                   // USELESS
    &not_implemented_syscall,   // exit
    &not_implemented_syscall,   // fork
    &read,                      // read
    &write,                     // write (not implemented)
    &open,                      // open
    &close,                     // close
    &not_implemented_syscall    // getpid
};

int num_syscalls = 8;
