#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>
 
size_t strlen(const char* string)
{
	size_t result = 0;
	while ( string[result] )
		result++;
	return result;
}
