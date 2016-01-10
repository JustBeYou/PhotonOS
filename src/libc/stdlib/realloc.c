#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

void *realloc(void *p, size_t size)
{
#if defined(__is_photon_kernel)
    return krealloc(p, size);
#elif defined(__STDC_HOSTED__)
    void *new_p = malloc(size);
    Llist_t *chunk = get_chunk(user_heap, p);
    mem_chunk_t *data = (mem_chunk_t*) chunk->data;
    size_t chunk_size = data->size;
    
    for (size_t i = 0; i < chunk_size; i++) {
        ((char*) new_p)[i] = ((char*) p)[i];
    }
    free(p);
    
    return new_p;
#endif
}
