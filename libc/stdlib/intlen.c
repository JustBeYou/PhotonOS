#include <stdlib.h>

int intlen(int n, int base)
{
	int len;
	for(len = 0; n != 0; n /= base, len++) {}
	return len;
}
