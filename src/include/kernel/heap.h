#ifndef _heap_h
#define _heap_h

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <linked_list.h>

#define DEFAULT_HEAP_SIZE 0x1000000

typedef struct mem_chunk {
    char used;
    size_t size;
} mem_chunk_t;

typedef struct mem_heap {
    int magic;
    size_t mem_size;
    size_t mem_used;
    size_t mem_free;
    Llist_t *head;
    /***
    * Usable memory of the heap starts at
    * mem_heap->head + sizeof(Llist_t) + sizeof(mem_chunk_t).
    ***/
} mem_heap_t;

#define MEM_HEADER_SIZE (sizeof(Llist_t) + sizeof(mem_chunk_t))

void init_heap(size_t size);

// Memory management for kernel heap only
void *kmalloc(size_t size, int align, size_t *phys);
void *krealloc(void *p, size_t size);
void kfree(void *p);

Llist_t *get_chunk(mem_heap_t *heap, void *p);
Llist_t *split_mem_chunk(mem_heap_t *heap, Llist_t *chunk, size_t size);
void glue_mem_chunk(mem_heap_t *heap, Llist_t *l_chunk, Llist_t *r_chunk);

Llist_t *find_mem_chunk(mem_heap_t *heap, size_t size);
Llist_t *alloc_mem_chunk(mem_heap_t *heap, size_t size);
void free_mem_chunk(mem_heap_t *heap, Llist_t *mem);

#endif
