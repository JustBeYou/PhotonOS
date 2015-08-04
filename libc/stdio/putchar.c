#include <stdio.h>
#include <vga.h>
#include <io.h>

void putchar(char c)
{	
	write_char(c);
}
