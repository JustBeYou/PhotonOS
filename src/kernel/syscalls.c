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

void *syscalls[] = {
    &useless, // USELESS
    NULL, // exit
    NULL, // fork
    NULL, // read
    NULL, // write
    NULL, // open
    NULL, // close
    NULL  // getpid
};

int num_syscalls = 8;
