#include <stdio.h>
#include <pit.h>
#include <tty.h>

int getch()
{
	for (;;) {
		if (((uint8_t*)stdin)[in_size] != 0) {
			in_size++;
			break;
		}
	}
	int c = ((uint8_t*)stdin)[in_size - 1];
	
	return c;
}
