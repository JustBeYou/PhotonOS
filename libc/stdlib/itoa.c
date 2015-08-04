#include <stdlib.h>
#include <string.h>

static void add_hex_prefix(char *buf)
{
	int len = strlen(buf);
	for (int i = len - 1; i >= 0; i--) {
		buf[i + 2] = buf[i];
	}
	buf[len + 2] = '\0';
	buf[0] = '0';
	buf[1] = 'x';
}

void itoa(char *buf, unsigned long int n, int base)
{
	unsigned long int tmp;
	int i;

	tmp = n;
	i = 0;

	do {
		tmp = n % base;
		buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
	} while (n /= base);
	buf[i] = '\0';

	strrev(buf);
	if (base == 16) {
		add_hex_prefix(buf);
	}
}
