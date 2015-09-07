#include <stddef.h>
#include <stdint.h>
#include <io.h>
#include <system.h>
#include <pmm.h>
#include <vmm.h>
#include <string.h>
#include <kheap.h>

extern void write_cr3();
extern void write_cr0();
extern uint32_t read_cr3();
extern uint32_t read_cr0();
extern uint32_t *frames;
extern uint32_t nframes;
extern int paging_enabled;

page_directory_t *kernel_directory;
page_directory_t *current_directory;

void init_vmm(uint32_t mem_size)
{
    // Register interrupt handler
    register_interrupt_handler(14, &page_fault_handler);

    // Alloc space for kernel directory
    kernel_directory = kmalloc(sizeof(page_directory_t), 1, 0);
    
    // Empty it
    memset(kernel_directory, 0, sizeof(page_directory_t));

    // Set current directory to kernel diretory
    current_directory = kernel_directory;

    // Map the memory
    uint32_t mem_addr = 0x0;
    uint32_t frame_addr = 0x0;
    while (mem_addr < (size_t) &kernel_end + 0x10000) {
        frame_addr = find_frame();
	    map(mem_addr, frame_addr, 1, 1);
	    mem_addr += 32;
    }
    // Switch current directory
    switch_page_directory(kernel_directory);

    // Enable paging
    enable_paging();
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    write_cr3((uint32_t) &(dir->phys_tables));
}

void enable_paging()
{
    uint32_t cr0 = read_cr0();
    cr0 |= 0x80000000;
    write_cr0(cr0);
    paging_enabled = 1;
}

void map(uint32_t va, uint32_t pa, uint32_t rw, uint32_t us)
{
    page_t *page = get_page(va, current_directory);
    create_page(page, pa, rw, us);
}

void unmap(uint32_t va)
{
    page_t *page = get_page(va, current_directory);
    free_frame(page->frame);
    memset(page, 0, sizeof(page_t));
    asm volatile ("invlpg (%0)" : : "a" (va));
}

void create_page(page_t *page, uint32_t frame, uint32_t rw, uint32_t us)
{
    page->present = 1;
    page->rw = rw;
    page->us = us;
    page->frame = frame;
}

page_t *get_page(uint32_t address, page_directory_t *dir)
{
    int page_index = address / 0x1000;
    int table_index = page_index / 1024;

    if (dir->tables[table_index] != 0) {
        return &(dir->tables[table_index]->pages[page_index]);
    } else {
        uint32_t phys_table_addr;
        dir->tables[table_index] = kmalloc(sizeof(page_table_t), 
	                                   1, &phys_table_addr);
        memset((void*) dir->tables[table_index], 0, sizeof(page_table_t));
	dir->phys_tables[table_index] = phys_table_addr | 0x7;
	return &(dir->tables[table_index]->pages[page_index]);
    }
    return NULL;
}


void debug_mem_mngr()
{
    printk("[DEBUG] Testing memory manager...\n");
    printk("page %d page_table %d page_directory %d\n", sizeof(page_t), sizeof(page_table_t), sizeof(page_directory_t));
    printk("Frame allocator test:\n");
    uint32_t addr_a = find_frame();
    uint32_t addr_b = find_frame();

    int index_a = FRAME_INDEX(addr_a);
    int index_b = FRAME_INDEX(addr_b);

    printk("Frame A: %x, index: %d, used: %d\n", addr_a, index_a, frames[index_a]);
    printk("Frame B: %x, index: %d, used: %d\n", addr_b, index_b, frames[index_b]);

    free_frame(addr_a);

    printk("After free, value of frame A: %d\n", frames[index_a]);

    uint32_t addr_c = find_frame();
    uint32_t addr_d = find_frame();

    int index_c = FRAME_INDEX(addr_c);
    int index_d = FRAME_INDEX(addr_d);

    printk("Frame C: %x, index: %d, used: %d\n", addr_c, index_c, frames[index_c]);
    printk("Frame D: %x, index: %d, used: %d\n", addr_d, index_d, frames[index_d]);

    if (index_c != index_a || addr_c != addr_a) {
	panic("[ERROR] Page frame allocator crash.\n", __LINE__, __FILE__);
    } else {
        printk("[INFO] Page frame allocator works fine.\n");
    }
    
    free_frame(addr_a);
    free_frame(addr_b);
    free_frame(addr_c);
    free_frame(addr_d);

    printk("[INFO] Testing page directory.\n");
    if (current_directory->tables[0]->pages[0].frame != 0x0) {
        panic("[ERROR] Map function error.", __LINE__, __FILE__);
    } else {
        printk("[INFO] All is good.\n");
	page_t *page3 = &(current_directory->tables[0]->pages[2]);
	printk("Page 3: frame: %x present: %x rw: %x us: %x\n", page3->frame, page3->present, page3->rw, page3->us);
    }

    void *error = kmalloc(sizeof(int), 0, 0);
    printk("%d", (uint32_t*) error);
}

void page_fault_handler(registers_t *regs)
{
    uint32_t cr2;
    asm volatile ("mov %%cr2, %0": "=r" (cr2));
    printk("Page fault at %x, faulting address %x\n", regs->eip, cr2);
    printk("Error code %x\n", regs->err_code);
    panic("Core dumb.\n", __LINE__, __FILE__);
}
