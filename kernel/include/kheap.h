#ifndef _kheap_h
#define _kheap_h

#include <stdint.h>
#include <stddef.h>
#include <list.h>
#include <system.h>

#define KHEAP_START 0xC0000000
#define KHEAP_INIT_SIZE 0x100000
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_MIN_SIZE 0x70000

size_t placement_address;

typedef struct {
	uint32_t magic;
	uint8_t is_hole;
	uint32_t size;
} header_t;

typedef struct {
	uint32_t magic;
	header_t *header;
} footer_t;

typedef struct {
	list_t index;
	uint32_t start_address;
	uint32_t end_address;
	uint32_t max_address;
	uint8_t supervisor;
	uint8_t readonly;
} heap_t;

heap_t *kheap;

/**
  * Create a new heap.
  */
heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);

/**
  * Allocate a block of memory of 'size'. If page_align == 1, it creates that block starting on a page boundary.
  */
void *alloc(uint32_t size, uint8_t page_align, heap_t *heap_t);

/**
  * Relase a block of memory.
  */
void free(void *p, heap_t *heap);

/**
  * Find the smallest hole that will fit. 
  */
int32_t find_hole(uint32_t size, uint8_t page_align, heap_t *heap);

/**
  * Header_t 'less than' checker.
  */
int8_t header_t_lessthan(type_t a, type_t b);

/**
  * Expand the heap.
  */
void expand(uint32_t new_size, heap_t *heap);

/**
  * Contract the heap.
  */
uint32_t contract(uint32_t new_size, heap_t *heap);

/**
  * Alloc a block of memory with specific options.
  */
size_t kmalloc_int(size_t sz, int align, size_t *phys);

/**
  * Alloc a block of memory with page aligned.
  */
size_t kmalloc_a(size_t sz);

/**
  * Alloc a block of memory and returns physical address.
  */
size_t kmalloc_p(size_t sz, size_t *phys);

/**
  * Alloc a block of memory with with page aligned and returns physical address.
  */
size_t kmalloc_ap(size_t sz, size_t *phys);

/**
  * Alloc a block of memory for kernel.
  */
size_t kmalloc(size_t sz);

void kfree(void *p);

#endif
