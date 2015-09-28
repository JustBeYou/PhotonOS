#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

extern void *stdin;
extern volatile uint32_t in_size;

int getch()
{
    while (true) {
        if (((uint8_t*)stdin)[in_size] != 0) {
            in_size++;
            break;
        }
    }
    int c = ((uint8_t*)stdin)[in_size - 1];
    return c;
}
