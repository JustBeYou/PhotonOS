/* Paging driver
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <paging.h>
#include <kheap.h>
#include <system.h>
#include <string.h>
#include <handlers.h>
#include <io.h>
#include <stdio.h>
#include <multiboot.h>

uint32_t *frames;
uint32_t nframes;

void set_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

void clear_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off); 
}

uint32_t test_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = INDEX_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

uint32_t first_frame()
{
	uint32_t i, j;
	for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] != 0xFFFFFFFF) {
			for (j = 0; j < 32; j++) {
				uint32_t toTest = 0x1 << j;
				if (!(frames[i] & toTest)) {
					return i * 4 * 8 + j;
				}
			}
		}
	}
	return -1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	if (page->frame != 0) {
		return;
	} else {
		uint32_t idx = first_frame();
		if (idx == (uint32_t)-1) {
			panic("No free frames!", __LINE__, __FILE__);
		}
		set_frame(idx * 0x1000);
		page->present = 1;
		page->rw = (is_writeable)?1:0;
		page->user = (is_kernel)?0:1;
		page->frame = idx;
	}
}

void free_frame(page_t *page)
{
	uint32_t frame;
	if (!(frame = page->frame)) {
		return;
	} else {
		clear_frame(frame);
		page->frame = 0x0;
	}
}

void init_paging(multiboot *mboot_ptr)
{
	placement_address = (size_t)&kernel_end;
	kheap = NULL;
	
	uint32_t mem_in_mb = mboot_ptr->mem_upper / 1024 + 2;
	uint32_t mem_in_kb = mem_in_mb * 1024;
	
	mem_size_mb = mem_in_mb;
	mem_size_kb = mem_in_kb;
	
	nframes = mem_in_kb / 4;
	frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));
	
	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	kernel_directory->physicalAddr = (uint32_t) kernel_directory->tablesPhysical;
	
	unsigned int i = 0;
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000) {
		get_page(i, 1, kernel_directory);
	}

	i = 0;
	while (i < (size_t)&kernel_end + KHEAP_INIT_SIZE) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 1);
		i += 0x1000;
	}
	
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 1);
	}	
	register_interrupt_handler(14, &page_fault);
	current_directory = clone_directory(kernel_directory);
	switch_page_directory(kernel_directory);
	enable_paging();

	kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INIT_SIZE, 0xCFFFF000, 0, 0);
}

void switch_page_directory(page_directory_t *dir)
{
	current_directory = dir;
	asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
}

void enable_paging()
{
	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
	address /= 0x1000;
	uint32_t table_idx = address / 1024;
	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[address % 1024];
	} else if (make) {
		uint32_t tmp;
		dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
		memset(dir->tables[table_idx], 0, 0x1000);
		dir->tablesPhysical[table_idx] = tmp | 0x7;
		return &dir->tables[table_idx]->pages[address % 1024];
	} else {
		return 0;
	}
}

void page_fault(registers_t *regs)
{
	cli();
	print_regs(regs);
	
	printf("Kernel heap: %x\n", kheap);
	
	uint32_t fault_address;
	asm volatile("mov %%cr2, %0" : "=r" (fault_address));
	
		// The error code gives us details of what happened.
	int present	= !(regs->err_code & 0x1); // Page not present
	int rw = regs->err_code & 0x2;			  // Write operation?
	int us = regs->err_code & 0x4;			  // Processor was in user-mode?
	int reserved = regs->err_code & 0x8;	  // Overwritten CPU-reserved bits of page entry?
	int id = regs->err_code & 0x10;			 // Caused by an instruction fetch?
	
	printf("Page fault at %x! Id = %x ( ", fault_address, id);
	if (present) printf("present ");
	if (rw) printf("read-only ");
	if (us) printf("user-mode ");
	if (reserved) printf("reserved ");
	printf(")\n");
	panic("Core dumped.", __LINE__, __FILE__);
}

page_directory_t *clone_directory(page_directory_t *src)
{
	uint32_t phys;
	page_directory_t *dir = (page_directory_t*) kmalloc_ap(sizeof(page_directory_t), &phys);
	memset(dir, 0, sizeof(page_directory_t));
	
	uint32_t offset = (uint32_t) dir->tablesPhysical - (uint32_t) dir;
	dir->physicalAddr = phys + offset;
	
	int i;
	for (i = 0; i < 1024; i++) {
		if (!src->tables[i])
			continue;
		
		if (kernel_directory->tables[i] == src->tables[i]) {
			dir->tables[i] = src->tables[i];
			dir->tablesPhysical[i] = src->tablesPhysical[i];
		} else {
			uint32_t page_phys;
			dir->tables[i] = clone_table((src->tables), &page_phys);
			dir->tablesPhysical[i] = page_phys | 0x07;
		}
	}
	return dir;
}

page_table_t *clone_table(page_table_t *src, uint32_t *physAddr)
{
	page_table_t *table = (page_table_t*) kmalloc_ap(sizeof(page_table_t), physAddr);
	memset(table, 0, sizeof(page_table_t));
	
	int i;
	for (i = 0; i < 1024; i++) {
		if (!src->pages[i].frame)
			continue;
		alloc_frame(&table->pages[i], 0, 0);
		if (src->pages[i].present) 
			table->pages[i].present = 1;
		if (src->pages[i].rw)
			table->pages[i].rw = 1;
		if (src->pages[i].user)
			table->pages[i].user = 1;
		if (src->pages[i].accessed)
			table->pages[i].accessed = 1;
		if (src->pages[i].dirty)
			table->pages[i].dirty = 1;
			
		copy_page_physical(src->pages[i].frame * 0x1000, table->pages[i].frame * 0x1000);
	}
	return table;
}

void move_stack(void *new_stack, uint32_t size)
{
	for (uint32_t i = (uint32_t) new_stack; i >= (uint32_t) new_stack - size; i -= 0x1000) {
		alloc_frame(get_page(i, 1, current_directory), 0, 1);
	}
	flush_tlb();
	
	uint32_t old_stack_pointer; 
	asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
 	uint32_t old_base_pointer; 
 	asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));
 	
 	uint32_t stack_offset = (uint32_t) new_stack - init_esp;
 	uint32_t new_stack_pointer = old_stack_pointer + stack_offset;
 	uint32_t new_base_pointer = old_base_pointer + stack_offset;
 	
 	memcpy((void*) new_stack_pointer, (void*) old_stack_pointer, init_esp - old_stack_pointer);
 	
	for(uint32_t i = (uint32_t) new_stack; i > (uint32_t) new_stack - size; i -= 4) {
		uint32_t tmp = *(uint32_t*) i;
		if (( old_stack_pointer < tmp) && (tmp < init_esp)) {
			tmp = tmp + stack_offset;
			uint32_t *tmp2 = (uint32_t*) i;
			*tmp2 = tmp;
		}
	} 
	
	asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
	asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}
