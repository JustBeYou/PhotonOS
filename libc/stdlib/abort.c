#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__))
void abort(void)
{
#if __STDC_HOSTED__
	// TODO: Properly implement abort().
	exit(1);
#elif defined(__is_photon_kernel)
	// TODO: Add proper kernel panic.
	printf("Kernel Panic: abort()\n");
	while ( 1 ) { }
#else
#error "You need to implement abort() in this freestanding environment."
#endif
	__builtin_unreachable();
}
