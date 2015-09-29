#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

__attribute__((__noreturn__))
void abort(void)
{
#if __STDC_HOSTED__
    // TODO: Properly implement abort().
    exit(1);
#elif defined(__is_photon_kernel)
    panic("Kernel aborted!\n", __LINE__, __FILE__);
#else
#error "You need to implement abort() in this freestanding environment."
#endif
    __builtin_unreachable();
}
