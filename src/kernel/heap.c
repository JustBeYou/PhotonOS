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

#include <i386/vmm.h>
#include <i386/pmm.h>
#include <kernel/heap.h>
#include <kernel/io.h>

mem_heap_t *default_heap;
Llist_t *default_heap_head;
mem_heap_t *kernel_heap;
Llist_t *kernel_heap_head;

extern size_t placement_addr;
char heap_initialized = 0;

void init_heap(size_t size)
{
    default_heap = kmalloc(sizeof(mem_heap_t), 0, 0);
    default_heap_head = kmalloc(sizeof(Llist_t), 0, 0);
    mem_chunk_t *temp_chunk = kmalloc(sizeof(mem_chunk_t), 0, 0);

    default_heap->magic = 0xbeefbeef;
    default_heap->mem_size = size;
    default_heap->mem_used = 0x0;
    default_heap->mem_free = size - sizeof(default_heap)
                            - sizeof(default_heap_head)
                            - sizeof(temp_chunk);
    default_heap->head = default_heap_head;

    default_heap_head->prev = NULL;
    default_heap_head->next = NULL;
    default_heap_head->data = (void*) temp_chunk;

    temp_chunk->used = 0;
    temp_chunk->size = default_heap->mem_free;

    kernel_heap = default_heap;
    kernel_heap_head = default_heap_head;

    heap_initialized = 1;
}


// Memory management for kernel heap only
void *kmalloc(size_t size, int align, size_t *phys)
{
    if (heap_initialized) {
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
        size_t ret_addr = placement_addr;
        placement_addr += size;
        void *ret_p = (void*) ret_addr;
        return ret_p;
    }
    return NULL;
}

void *krealloc(void *p, size_t size)
{
    Llist_t *chunk = get_chunk(kernel_heap, p);
    if (chunk == NULL) {
        return NULL;
    }

    void *new_p = kmalloc(size, 0, 0);
    mem_chunk_t *data = (mem_chunk_t*) chunk->data;
    size_t chunk_size = data->size;

    for (size_t i = 0; i < chunk_size; i++) {
        ((char*) new_p)[i] = ((char*) p)[i];
    }
    kfree(p);

    kernel_heap->mem_used += size - chunk_size;
    kernel_heap->mem_free -= size - chunk_size;
    return new_p;
}

void kfree(void *p)
{
    if (heap_initialized) {
        Llist_t *chunk = (Llist_t*) ((size_t) p - MEM_HEADER_SIZE);
        //mem_chunk_t *data = (mem_chunk_t*) chunk->data;
        free_mem_chunk(kernel_heap, chunk);
    } else {
        return;
    }
}


Llist_t *get_chunk(mem_heap_t *heap, void *p)
{
    Llist_t *head = heap->head;

    while (head != NULL) {
        if ((size_t) p == (size_t) (head + MEM_HEADER_SIZE)) {
            return head;
        }
        head = head->next;
    }

    return NULL;
}

Llist_t *split_mem_chunk(mem_heap_t *heap, Llist_t *chunk, size_t size)
{
    /***
    * I. |header-----free memory-----|
                chunk^
    * II. |header-used memory||header--free memory--|
            left chunk^         right chunk^
    ***/
    mem_chunk_t *chunk_data = chunk->data;
    Llist_t *next = chunk->next;

    if (size >= chunk_data->size - MEM_HEADER_SIZE) {
        return NULL;
    }

    size_t new_chunk_size = chunk_data->size - size - MEM_HEADER_SIZE;
    chunk_data->size = size;
    chunk->next = (Llist_t*) ((size_t) chunk + MEM_HEADER_SIZE + chunk_data->size);
    chunk->next->data = (void*) chunk->next + sizeof(Llist_t);

    mem_chunk_t *new_chunk_data = (mem_chunk_t*) chunk->next->data;
    new_chunk_data->used = chunk_data->used;
    new_chunk_data->size = new_chunk_size;
    chunk->next->next = next;
    chunk->next->prev = chunk;

    if (next != NULL) {
        next->prev = chunk->next;
    }

    heap->mem_used += MEM_HEADER_SIZE;
    heap->mem_free -= MEM_HEADER_SIZE;
    return chunk;
}

void glue_mem_chunk(mem_heap_t *heap, Llist_t *l_chunk, Llist_t *r_chunk)
{
    Llist_t *next = r_chunk->next;
    Llist_t *prev = l_chunk->prev;

    mem_chunk_t *l_chunk_data = (mem_chunk_t*) l_chunk->data;
    mem_chunk_t *r_chunk_data = (mem_chunk_t*) r_chunk->data;

    size_t new_size = l_chunk_data->size + r_chunk_data->size + MEM_HEADER_SIZE;
    l_chunk_data->size = new_size;

    l_chunk->next = next;
    l_chunk->prev = prev;

    if (next != NULL) {
        next->prev = l_chunk;
    }

    if (prev != NULL) {
        prev->next = l_chunk;
    }

    heap->mem_used -= MEM_HEADER_SIZE;
    heap->mem_free += MEM_HEADER_SIZE;
}


Llist_t *find_mem_chunk(mem_heap_t *heap, size_t size)
{
    if (heap->mem_free < size + MEM_HEADER_SIZE) {
        return NULL;
    }

    Llist_t *chunk = heap->head;
    mem_chunk_t *header = chunk->data;

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
        if (chunk_data->size <= size + MEM_HEADER_SIZE) {
            chunk_data->used = 1;
            heap->mem_free -= size;
            heap->mem_used += size;

            return free_chunk;
        }

        Llist_t *allocated_chunk = split_mem_chunk(heap, free_chunk, size);

        if (allocated_chunk == NULL) {
            return NULL;
        }

        chunk_data = (mem_chunk_t*) allocated_chunk->data;
        heap->mem_free -= size;
        heap->mem_used += size;

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
            glue_mem_chunk(heap, mem->prev, mem);
            mem = new_chunk;
        }
    }

    if (mem->next != NULL) {
        mem_chunk_t *next_data = (mem_chunk_t*) mem->next->data;
        if (next_data->used == 0) {
            glue_mem_chunk(heap, mem, mem->next);
        }
    }
}
