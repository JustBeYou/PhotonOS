#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

void *calloc(size_t n)
{
    void *new_p = malloc(n);
    memset(new_p, 0, n);
    return new_p;
}
