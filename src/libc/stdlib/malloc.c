#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

#include <kernel/heap.h>

mem_heap_t *user_heap;
Llist_t *user_free_mem_head;
mem_chunk_t *user_mem_chunk_head;
char malloc_initialized = 0;

void *malloc(size_t n)
{
#if defined(__is_photon_kernel)
    return kmalloc(n, 0, 0);
#elif defined(__STDC_HOSTED__)

#endif
}
