/* Kernel heap driver
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <kheap.h>
#include <paging.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

int32_t find_hole(uint32_t size, uint8_t page_align, heap_t *heap) 
{
	uint32_t iterator = 0;
	while (iterator < heap->index.size) {
		header_t *header = (header_t*) lookup_list(iterator, &heap->index);
		if (page_align > 0) {
			uint32_t location = (uint32_t) header;
			int32_t offset = 0;
			if (((location + sizeof(header_t)) & 0xFFFFF000) != 0) {
				offset = 0x1000 - (location + sizeof(header_t) % 0x1000);
			}
			int32_t hole_size = (int32_t) header->size - offset;
			if (hole_size >= (int32_t) size) {
				break;
			}
		} else if (header->size >= size) {
			break;
		}
		iterator++;
	}
	if (iterator == heap->index.size) {
		return -1;
	} else {
		return iterator;
	}
}

int8_t header_t_lessthan(type_t a, type_t b)
{
	return (((header_t*)a)->size < ((header_t*)b)->size)?1:0;
}

heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly)
{
	heap_t *heap = (heap_t*) kmalloc(sizeof(heap_t));
	ASSERT((start % 0x1000 == 0));
	ASSERT((end % 0x1000 == 0));

	heap->index = place_list((type_t*) start, HEAP_INDEX_SIZE, &header_t_lessthan);
	
	start += sizeof(type_t) * HEAP_INDEX_SIZE;
	
	if ((start & 0xFFFFF000) != 0) {
		start &= 0xFFFFF000;
		start += 0x1000;
	}
	
	heap->start_address = start;
	heap->end_address = end;
	heap->max_address = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;
	
	header_t *hole = (header_t*) start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_list((type_t) hole, &heap->index);
	
	return heap;
}

void *alloc(uint32_t size, uint8_t page_align, heap_t *heap)
{
	uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t);
	int32_t iterator = find_hole(new_size, page_align, heap);
	
	if (iterator == (int32_t) -1) {
		uint32_t old_length = heap->end_address - heap->start_address;
		uint32_t old_end_address = heap->end_address;

		expand(old_length+new_size, heap);
		uint32_t new_length = heap->end_address-heap->start_address;

		iterator = 0;

		int32_t idx = -1;
		uint32_t value = 0x0;
		while (iterator < (int32_t) heap->index.size)
		{
		    uint32_t tmp = (uint32_t)lookup_list(iterator, &heap->index);
		    if (tmp > value)
		    {
			 value = tmp;
			 idx = iterator;
		    }
		    iterator++;
		}

		if (idx == -1)
		{
		    header_t *header = (header_t *)old_end_address;
		    header->magic = HEAP_MAGIC;
		    header->size = new_length - old_length;
		    header->is_hole = 1;
		    footer_t *footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));
		    footer->magic = HEAP_MAGIC;
		    footer->header = header;
		    insert_list((void*)header, &heap->index);
		}
		else
		{
		    header_t *header = lookup_list(idx, &heap->index);
		    header->size += new_length - old_length;
		    footer_t *footer = (footer_t *) ( (uint32_t)header + header->size - sizeof(footer_t) );
		    footer->header = header;
		    footer->magic = HEAP_MAGIC;
		}
		
		return alloc(size, page_align, heap);
	}
	
	header_t *orig_hole_header = (header_t *)lookup_list(iterator, &heap->index);
	uint32_t orig_hole_pos = (uint32_t) orig_hole_header;
	uint32_t orig_hole_size = orig_hole_header->size;
		
	if (orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}
	
	if (page_align && (orig_hole_pos & 0xFFFFF000)) {
		uint32_t new_location = orig_hole_pos + 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
		header_t *hole_header = (header_t*) orig_hole_pos;
		hole_header->size = 0x1000;
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		footer_t *hole_footer = (footer_t*) ((uint32_t) new_location - sizeof(footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole_header;
		orig_hole_pos = new_location;
		orig_hole_size = orig_hole_size - hole_header->size;
	} else {
		remove_list(iterator, &heap->index);
	}
	
	header_t *block_header = (header_t*) orig_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;
	
	footer_t *block_footer = (footer_t*) (orig_hole_pos + sizeof(header_t) + size);
	block_footer->header = block_header;
	
	if (orig_hole_size - new_size > 0) {
		header_t *hole_header = (header_t*) (orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		hole_header->size = orig_hole_size - new_size;
		footer_t *hole_footer = (footer_t*) ((uint32_t) hole_header + orig_hole_size - new_size - sizeof(footer_t));
		
		if ((uint32_t) hole_footer < heap->end_address) {
			hole_footer->magic = HEAP_MAGIC;
			hole_footer->header = hole_header;			
		}
		insert_list((void*)hole_header, &heap->index);
	}
	return (void*) ((uint32_t) block_header + sizeof(header_t));
}

void free(void *p, heap_t *heap)
{
    // Exit gracefully for null pointers.
    if (p == 0)
	   return;

    // Get the header and footer associated with this pointer.
    header_t *header = (header_t*) ( (uint32_t)p - sizeof(header_t) );
    footer_t *footer = (footer_t*) ( (uint32_t)header + header->size - sizeof(footer_t) );

    // Sanity checks.
    ASSERT(header->magic == HEAP_MAGIC);
    ASSERT(footer->magic == HEAP_MAGIC);

    // Make us a hole.
    header->is_hole = 1;

    // Do we want to add this header into the 'free holes' index?
    char do_add = 1;

    // Unify left
    // If the thing immediately to the left of us is a footer...
    footer_t *test_footer = (footer_t*) ( (uint32_t)header - sizeof(footer_t) );
    if (test_footer->magic == HEAP_MAGIC &&
	   test_footer->header->is_hole == 1)
    {
	   uint32_t cache_size = header->size; // Cache our current size.
	   header = test_footer->header;	// Rewrite our header with the new one.
	   footer->header = header;		// Rewrite our footer to point to the new header.
	   header->size += cache_size;	  // Change the size.
	   do_add = 0;				   // Since this header is already in the index, we don't want to add it again.
    }

    // Unify right
    // If the thing immediately to the right of us is a header...
    header_t *test_header = (header_t*) ( (uint32_t)footer + sizeof(footer_t) );
    if (test_header->magic == HEAP_MAGIC &&
	   test_header->is_hole)
    {
	   header->size += test_header->size; // Increase our size.
	   test_footer = (footer_t*) ( (uint32_t)test_header + // Rewrite it's footer to point to our header.
							 test_header->size - sizeof(footer_t) );
	   footer = test_footer;
	   // Find and remove this header from the index.
	   uint32_t iterator = 0;
	   while ( (iterator < heap->index.size) &&
			 (lookup_list(iterator, &heap->index) != (void*)test_header) )
		  iterator++;

	   // Make sure we actually found the item.
	   ASSERT(iterator < heap->index.size);
	   // Remove it.
	   remove_list(iterator, &heap->index);
    }

    // If the footer location is the end address, we can contract.
    if ( (uint32_t)footer+sizeof(footer_t) == heap->end_address)
    {
	   uint32_t old_length = heap->end_address-heap->start_address;
	   uint32_t new_length = contract( (uint32_t)header - heap->start_address, heap);
	   // Check how big we will be after resizing.
	   if (header->size - (old_length-new_length) > 0)
	   {
		  // We will still exist, so resize us.
		  header->size -= old_length-new_length;
		  footer = (footer_t*) ( (uint32_t)header + header->size - sizeof(footer_t) );
		  footer->magic = HEAP_MAGIC;
		  footer->header = header;
	   }
	   else
	   {
		  // We will no longer exist :(. Remove us from the index.
		  uint32_t iterator = 0;
		  while ( (iterator < heap->index.size) &&
				(lookup_list(iterator, &heap->index) != (void*)test_header) )
			 iterator++;
		  // If we didn't find ourselves, we have nothing to remove.
		  if (iterator < heap->index.size)
			 remove_list(iterator, &heap->index);
	   }
    }

    // If required, add us to the index.
    if (do_add == 1)
	   insert_list((void*)header, &heap->index);

}

void expand(uint32_t new_size, heap_t *heap)
{
	ASSERT(new_size > heap->end_address - heap->start_address);
	
	if ((new_size & 0xFFFFF000) != 0) {
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}
	
	ASSERT(heap->start_address + new_size <= heap->max_address);
	
	uint32_t old_size = heap->end_address-heap->start_address;
	uint32_t i = old_size;
	while (i < new_size) {
		alloc_frame( get_page(heap->start_address+i, 1, kernel_directory),
				(heap->supervisor)?1:0, (heap->readonly)?0:1);
		i += 0x1000;
	}
	heap->end_address = heap->start_address+new_size;
}

uint32_t contract(uint32_t new_size, heap_t *heap)
{
	ASSERT(new_size < heap->end_address - heap->start_address);
	
	if (new_size & 0x1000) {
		new_size &= 0x1000;
		new_size += 0x1000;
	}
	
	if (new_size < HEAP_MIN_SIZE) {
		new_size = HEAP_MIN_SIZE;
	}
	uint32_t old_size =  heap->end_address - heap->start_address;
	uint32_t i = old_size - 0x1000;
	
	while (new_size < i) {
		free_frame(get_page(heap->start_address + i, 0, kernel_directory));
		i -= 0x1000;
	}
	heap->end_address = heap->start_address + new_size;
	
	return new_size;
}

size_t kmalloc_int(size_t sz, int align, size_t *phys)
{
	if (kheap == NULL) {
		if (align == 1 && (placement_address & 0xFFFFF000)) {
			placement_address &= 0xFFFFF000;
			placement_address += 0x1000;
		}
		if (phys) {
			*phys = placement_address;
		}
		size_t tmp = placement_address;
		placement_address += sz;
		return tmp;
	} else {
		void *addr = alloc(sz, (uint8_t) align, kheap);
		if (phys != NULL) {
			page_t *page = get_page((uint32_t) addr, 0, kernel_directory);
			*phys = page->frame * 0x1000 + ((uint32_t) addr & 0xFFF);
		}
		return (uint32_t) addr;
	}
}

size_t kmalloc_a(size_t sz)
{
	return kmalloc_int(sz, 1, 0);
}

size_t kmalloc_p(size_t sz, size_t *phys)
{
	return kmalloc_int(sz, 0, phys);
}

size_t kmalloc_ap(size_t sz, size_t *phys)
{
	return kmalloc_int(sz, 1, phys);
}

size_t kmalloc(size_t sz)
{
	return kmalloc_int(sz, 0, 0);
}

void kfree(void *p)
{
	if (p != NULL) { 
		free(p, kheap);
		p = NULL;
	} 
}
