#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

#include <kernel/heap.h>

void *realloc(void *p, size_t size)
{
#if defined(__is_photon_kernel)
    return krealloc(p, size);
#elif defined(__STDC_HOSTED__)

#endif
}
