/* Paging driver header
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * Contains declarations of functions for paging driver and paging structures.
 * paging is one of the memory management schemes by which a computer stores
 * and retrieves data from the secondary storage for use in main memory.
 * Structures:
 *  - page: structure used to store details about a block of memory
 *  - page table: structure used to store 1024 pages
 *  - page directory: structure used to store 1024 page entries and their
 *    physical addresses, in addition it stores physical address that will be loaded
 *    into CR3.
 */

#ifndef _paging_h
#define _paging_h

#include <stdint.h>
#include <stddef.h>
#include <handlers.h>
#include <system.h>

typedef struct page
{
	uint32_t present	 : 1;	// Page present in memory
	uint32_t rw			: 1;	// Read-only if clear, read write if set
	uint32_t user		 : 1;	// Supervisor level only if clear
	uint32_t accessed	: 1;	// Has the page been accessed since last refresh?
	uint32_t dirty		: 1;	// Has the page been written to since last refresh?
	uint32_t unused	  : 7;	// Amalgamation of unused and reserved bits
	uint32_t frame		: 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_table_t *tables[1024];
	uint32_t tablesPhysical[1024];
	uint32_t physicalAddr;
} page_directory_t;

page_directory_t *kernel_directory;
page_directory_t *current_directory;
uint32_t mem_size_mb;
uint32_t mem_size_kb;

extern void copy_page_physical(uint32_t src_frame, uint32_t dest_frame);
extern void flush_tlb();

/**
  * Initialize paging.
  */
void init_paging();

/**
  * Switch current page directory and load it.
  */
void switch_page_directory(page_directory_t *dir);

/**
  * It does what it does. :))) 
  */
void enable_paging();

/**
  * Get a free page.
  */
page_t *get_page(uint32_t address, int make, page_directory_t *dir);

/**
  * Page fault handler.
  */
void page_fault(registers_t *regs);

/**
  * Set frame bit.
  */
void set_frame(uint32_t frame_addr);

/**
  * Clear frame.
  */
void clear_frame(uint32_t frame_addr);

/**
  * Test the frame.
  */
uint32_t test_frame(uint32_t frame_addr);

/**
  * First free frame.
  */
uint32_t first_frame();

/**
  * Alloc first free frame to page.
  */
void alloc_frame(page_t *page, int is_kernel, int is_writeable);

/**
  * Free frame from page.
  */
void free_frame(page_t *page);

/**
  * Copy data from a directory and return it as a pointer to a directory allocated on the heap.
  */
page_directory_t *clone_directory(page_directory_t *src);

/**
  * Copy data from a page and return it.
  */
page_table_t *clone_table(page_table_t *src, uint32_t *physAddr);

/**
  * Simply move the stack...
  */
void move_stack(void *new_stack, uint32_t size);

#endif
