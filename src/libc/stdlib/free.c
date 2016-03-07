#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

#include <kernel/heap.h>

extern mem_heap_t *user_heap;

void free(void *p)
{
#if defined(__is_photon_kernel)
    kfree(p);
#elif defined(__STDC_HOSTED__)

#endif
}
