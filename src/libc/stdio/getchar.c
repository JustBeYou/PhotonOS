#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

int getchar()
{
    int c = getch();
    putchar(c);
    
    return c;
}
