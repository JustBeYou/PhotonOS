#ifndef _kheap_h
#define _kheap_h

#include <stdint.h>
#include <stddef.h>
#include <list.h>
#include <system.h>
#include <vmm.h>

#define HEAP_START 0xD0000000
#define HEAP_END     0xFFBFF000

typedef struct header
{
    struct header *prev, *next;
    uint32_t allocated : 1;
    uint32_t length : 31;
} header_t;

void init_heap ();

void *kmalloc (size_t size, int align, uint32_t *phys);

void kfree (void *p);

#endif
