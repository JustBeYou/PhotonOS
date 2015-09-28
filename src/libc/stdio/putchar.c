#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

void putchar(char c)
{    
    write_char(c);
}
