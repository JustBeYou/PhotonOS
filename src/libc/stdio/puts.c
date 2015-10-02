#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

int puts(const char* string)
{
    return printf("%s\n", string);
}
