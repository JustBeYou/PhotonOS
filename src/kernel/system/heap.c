/* Kernel heap driver
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

// TODO: resolve size recalculation bug after kfree()

extern uint32_t placement_addr;

int kheap_initialized = 0;
mem_heap_t *kernel_heap;
Llist_t *kernel_free_mem_head;
mem_chunk_t *free_mem_chunk_head;

void init_heap ()
{
    kernel_heap = kmalloc(sizeof(mem_heap_t), 0, 0);
    kernel_free_mem_head = kmalloc(sizeof(Llist_t), 0, 0);
    free_mem_chunk_head = kmalloc(sizeof(mem_chunk_t), 0, 0);
    
    kernel_heap->magic = 0xbeefbeef;
    kernel_heap->head = kernel_free_mem_head;
    kernel_heap->head->prev = NULL;
    kernel_heap->head->next = NULL;
    kernel_heap->head->data = (void*) free_mem_chunk_head;
    free_mem_chunk_head->used = 0;
    free_mem_chunk_head->size = KERNEL_HEAP_SIZE;
    kernel_heap->mem_size = KERNEL_HEAP_SIZE;
    kernel_heap->mem_free = KERNEL_HEAP_SIZE;
    kernel_heap->mem_used = 0;
    kheap_initialized = 1;
}

/* Kernel only allocator */
void *kmalloc(size_t size, int align, uint32_t *phys)
{
    if (kheap_initialized) {
        Llist_t *mem_chunk = alloc_mem_chunk(kernel_heap, size);
        if (mem_chunk == NULL) {
            return NULL;
        }
        
        void *ret_p = (void*) ((size_t) mem_chunk + MEM_HEADER_SIZE);
        return ret_p;
    } else {
        if (align && (placement_addr & 0xFFFFF000)) {
            placement_addr &= 0xFFFFF000;
	        placement_addr += 0x1000;
	    }
    
	    if (phys) {
            *phys = placement_addr;
	    }
	    uint32_t ret_addr = placement_addr;
        placement_addr += size;
	    void *ret_p = (void*) ret_addr;
	
        return ret_p;
    }
    return NULL;
}

void kfree(void *p)
{
    if (kheap_initialized) {
        Llist_t *chunk = (Llist_t*) ((size_t) p - MEM_HEADER_SIZE);
        free_mem_chunk(kernel_heap, chunk);
	} else {
	    return;
	}
}
/* ------------------------------------ */

Llist_t *split_mem_chunk(Llist_t *chunk, size_t size)
{
    mem_chunk_t *chunk_data = (mem_chunk_t*) chunk->data;

    Llist_t *next = chunk->next;
    
    if (size >= chunk_data->size) {
        return NULL;
    }
    
    size_t new_chunk_size = chunk_data->size - size - MEM_HEADER_SIZE;
    
    chunk_data->size = size;
    chunk->next = (Llist_t*) ((size_t) chunk + MEM_HEADER_SIZE + chunk_data->size);
    chunk->next->data = (void*) chunk->next + sizeof(int) * 3;

    mem_chunk_t *new_chunk_data = (mem_chunk_t*) chunk->next->data;
    new_chunk_data->used = chunk_data->used;
    new_chunk_data->size = new_chunk_size;
    chunk->next->next = next;
    chunk->next->prev = chunk;
    
    if (next != NULL) {
        next->prev = chunk->next;
    }
    
    return chunk;
}

void glue_mem_chunk(Llist_t *chunk1, Llist_t *chunk2)
{
    Llist_t *next = chunk2->next;
    Llist_t *prev = chunk1->prev;
    
    mem_chunk_t *chunk1_data = (mem_chunk_t*) chunk1->data;
    mem_chunk_t *chunk2_data = (mem_chunk_t*) chunk2->data;
    
    size_t new_size = chunk1_data->size + chunk2_data->size + MEM_HEADER_SIZE;
    chunk1_data->size = new_size;
    
    chunk1->next = next;
    chunk1->prev = prev;
    
    if (next != NULL) {
        next->prev = chunk1;
    }
    
    if (prev != NULL) {
        prev->next = chunk1;
    }
}

Llist_t *find_mem_chunk(mem_heap_t *heap, size_t size)
{
    if (heap->mem_free < size) {
        return NULL;
    }
    
    Llist_t *chunk = heap->head;
    mem_chunk_t *header = (mem_chunk_t*) chunk->data;
    
    while (chunk != NULL) {
        if (header->size >= size && header->used == 0) {
            return chunk;
        }
        chunk = chunk->next;
        
        if (chunk != NULL) {
            header = (mem_chunk_t*) chunk->data;
        }
    }
    
    return NULL;
}

Llist_t *alloc_mem_chunk(mem_heap_t *heap, size_t size)
{
    Llist_t *free_chunk = find_mem_chunk(heap, size);
    if (free_chunk == NULL) {
        return NULL;
    }
    
    mem_chunk_t *chunk_data = (mem_chunk_t*) free_chunk->data;
    
    if (chunk_data->size == size) {
        chunk_data->used = 1;
        heap->mem_free -= size;
        heap->mem_used += size;
        
        return free_chunk;
    } else {
        Llist_t *allocated_chunk = split_mem_chunk(free_chunk, size);
         
        if (allocated_chunk == NULL) {
            return NULL;
        }
        
        chunk_data = (mem_chunk_t*) allocated_chunk->data;
        heap->mem_free -= (size + MEM_HEADER_SIZE);
        heap->mem_used += (size + MEM_HEADER_SIZE);
        
        chunk_data->used = 1;
        return allocated_chunk;
    }
    
    return NULL;
}

void free_mem_chunk(mem_heap_t *heap, Llist_t *mem)
{
    mem_chunk_t *data = (mem_chunk_t*) mem->data;
    data->used = 0;
    heap->mem_free += data->size;
    heap->mem_used -= data->size;
    
    if (mem->prev != NULL) {
        mem_chunk_t *prev_data = (mem_chunk_t*) mem->prev->data;
        if (prev_data->used == 0) {
            Llist_t *new_chunk = mem->prev;
            glue_mem_chunk(mem->prev, mem);
            mem = new_chunk;
        }
    }
    
    if (mem->next != NULL) {
        mem_chunk_t *next_data = (mem_chunk_t*) mem->next->data;
        if (next_data->used == 0) {
            glue_mem_chunk(mem, mem->next);
        }
    }
}
