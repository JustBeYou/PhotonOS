#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

void exit(int status)
{
    while ( true )
    {
        // TODO: Implement the exit system call.
        (void) status;
    }
}
