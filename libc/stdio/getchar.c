#include <stdio.h>
#include <pit.h>

int getchar()
{
	int c = getch();
	putchar(c);
	
	return c;
}
