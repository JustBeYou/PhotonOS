#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

mem_heap_t *user_heap;
Llist_t *user_free_mem_head;
mem_chunk_t *user_mem_chunk_head;
char malloc_initialized = 0;

void *malloc(size_t n)
{
#if defined(__is_photon_kernel)
    return kmalloc(n, 0, 0);
#elif defined(__STDC_HOSTED__)
    if (!malloc_initialized) {
        // TODO: add malloc init
    }
    
    Llist_t *chunk = alloc_mem_chunk(user_heap, n);
    if (chunk == NULL) {
        return NULL;
    }
    
    void *ret_p = (void*) ((size_t) chunk - MEM_HEADER_SIZE);
    return ret_p;
#endif
}
