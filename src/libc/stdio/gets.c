#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

char *gets(char *str)
{
    int c = getch();
    int i = 0;
    while (c != '\n') {
        if (c != '\b') {
            str[i++] = c;
            putchar(c);
        } else if (c == '\b' && i > 0) {
            str[--i] = 0;
            putchar(c);
        }
        c = getch();
    }
    str[i] = '\0';
    putchar('\n');
    return str;
}
