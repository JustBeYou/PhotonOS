#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

extern mem_heap_t *user_heap;

void free(void *p)
{
#if defined(__is_photon_kernel)
    kfree(p);
#elif defined(__STDC_HOSTED__)
    Llist_t *chunk = (Llist_t*) ((size_t) p - MEM_HEADER_SIZE);
    free_mem_chunk(user_heap, chunk);
#endif
}
