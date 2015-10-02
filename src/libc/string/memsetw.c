#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

void *memsetw(void *s, int c, size_t n)
{
	size_t i;
	unsigned short *s1 = (unsigned short *) s;
	
	for (i = 0; i < n; i++)
	{
		s1[i] = (unsigned short) c;
	}
	return s;
}
