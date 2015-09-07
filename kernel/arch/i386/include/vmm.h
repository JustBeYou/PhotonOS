#ifndef _vmm_h
#define _vmm_h

#include <stddef.h>
#include <stdint.h>
#include <system.h>

typedef struct page {
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t us         : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t unused     : 7;
    uint32_t frame      : 20;
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t __attribute__((aligned(4096)));

typedef struct page_directory {
    page_table_t *tables[1024];
    uint32_t phys_tables[1024];
} page_directory_t __attribute__((aligned(4096)));

void init_vmm(uint32_t mem_size);
void switch_page_directory(page_directory_t *dir);
void enable_paging();
void map(uint32_t va, uint32_t pa, uint32_t rw, uint32_t us);
void unmap(uint32_t va);
void create_page(page_t *page, uint32_t frame, uint32_t rw, uint32_t us);
page_t *get_page(uint32_t address, page_directory_t *dir);
void debug_mem_mngr();
void page_fault_handler(registers_t *regs);

#endif
