#include <paging.h>
#include <kheap.h>
#include <system.h>
#include <string.h>
#include <isr.h>
#include <io.h>
#include <stdio.h>

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

void init_paging()
{
	uint32_t mem_end_page = 0x1000000; // temp. value ~ 16 MB
	
	nframes = mem_end_page / 0x1000;
	frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));
	
	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	current_directory = kernel_directory;
	
	unsigned int i = 0;
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000) {
		get_page(i, 1, kernel_directory);
	}

	i = 0;
	while (i < 0x200000 /* ~ 2 MB */) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}
	
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000) {
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
	}	
	register_interrupt_handler(14, &page_fault);
	switch_page_directory(kernel_directory);

	kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INIT_SIZE, 0xCFFFF000, 0, 0);
}

void switch_page_directory(page_directory_t *dir)
{
	current_directory = dir;
	asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
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
	panic("Page fault.", __LINE__, __FILE__);
}
