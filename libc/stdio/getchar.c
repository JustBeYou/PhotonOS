#include <stdio.h>
#include <pit.h>
#include <tty.h>

int getchar()
{
	int c = getch();
	putchar(c);
	
	return c;
}
