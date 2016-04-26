#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>
#include <kernel/io.h>

void say_hello()
{
    char buf[] = "Hello!\n";
    size_t len = strlen(buf);
    vga_write(buf, len);
}

void *syscalls[] = {
	&vga_write,
    &kb_read,
    &say_hello
};

int num_syscalls = 3;
