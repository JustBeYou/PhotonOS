#ifndef _vmm_h
#define _vmm_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>

#define PAGE_PRESENT 0x1
#define PAGE_READ_WRITE 0x2
#define PAGE_USER 0x4
#define PAGE_WRITE_THROUGH 0x8
#define PAGE_CACHE_DISABLE 0x16
#define PAGE_ACCESSED 0x32
#define PAGE_DIRTY 0x64
#define PAGE_GLOBAL 0x128

typedef struct page_table {
    size_t pages[1024];
} page_table_t;

typedef struct page_directory {
    size_t phys_tables[1024];
    page_table_t *virt_tables[1024];
} page_directory_t;

void init_vmm();
void set_current_directory(page_directory_t *dir);
void switch_page_directory(page_directory_t *dir);
void enable_paging();
void map(size_t va, size_t pa, size_t flags);
void map_area(uint32_t from_va, uint32_t to_va, uint32_t flags);
void unmap(size_t va);
void page_fault_handler(registers_t *regs);


#endif
