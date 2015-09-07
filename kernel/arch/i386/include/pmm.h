/* Paging driver header
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * Contains declarations of functions for pmm driver and pmm structures.
 * pmm is one of the memory management schemes by which a computer stores
 * and retrieves data from the secondary storage for use in main memory.
 * Structures:
 *  - page: structure used to store details about a block of memory
 *  - page table: structure used to store 1024 pages
 *  - page directory: structure used to store 1024 page entries and their
 *    physical addresses, in addition it stores physical address that will be loaded
 *    into CR3.
 */

#ifndef _pmm_h
#define _pmm_h

#include <stdint.h>
#include <stddef.h>

#define FRAME_SIZE 0x20
#define FRAME_INDEX(x) (x / FRAME_SIZE)
#define PAGE_ALIGN(x) (x & 0xFFFFF000)
#define FRAME_ADDR_FROM_INDEX(x) (x * FRAME_SIZE)

void init_pmm(uint32_t mem_size);
uint32_t find_frame();
void use_frame(uint32_t frame);
void free_frame(uint32_t frame);

#endif
