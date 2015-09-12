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
extern uint32_t placement_addr;
extern size_t kernel_end;

page_directory_t *kernel_directory;
page_directory_t *current_directory;

static void map_area(uint32_t from_va, uint32_t to_va)
{
    for (uint32_t va = from_va; va < to_va; va += 0x1000) {
        uint32_t pa = alloc_frame();
        map(va, pa, PAGE_READ_WRITE | PAGE_PRESENT);
    }
}

void init_vmm()
{
    register_interrupt_handler(14, page_fault_handler);

    kernel_directory = kmalloc(sizeof(page_directory_t), 1, 0);
    memset(kernel_directory, 0, sizeof(page_directory_t));
    
    for (int i = 0; i < 1024; i++) {
        kernel_directory->phys_tables[i] = 0x0 | PAGE_READ_WRITE;
    }
    
    current_directory = kernel_directory;
    
    // Map BIOS
    map_area(0x0, 0x10000);
    
    // Map Kernel
    map_area(0x10000, (uint32_t) &kernel_end);
    
    switch_page_directory(kernel_directory);
    enable_paging();
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    write_cr3(current_directory);
}

void enable_paging()
{
    uint32_t cr0 = read_cr0();
    cr0 |= 0x80000000;
    write_cr0(cr0);
    paging_enabled = 1;
    
    // Temp settings
    placement_addr = 0x20000;
}

void map(uint32_t va, uint32_t pa, uint32_t flags)
{
    uint32_t page_num = (va / 4096) % 1024;  
    uint32_t table_num = (va / 4096) / 1024;
    
    if (!current_directory->virt_tables[table_num]) {
        uint32_t phys;
        current_directory->virt_tables[table_num] = kmalloc(sizeof(page_table_t), 
                                                    1, 
                                                    &phys);
                        
        current_directory->phys_tables[table_num] = phys | PAGE_READ_WRITE | PAGE_PRESENT;
    }   
    current_directory->virt_tables[table_num]->pages[page_num] = pa | flags;
}

void unmap(uint32_t va)
{
    uint32_t page_num = (va / 4096) % 1024;  
    uint32_t table_num = (va / 4096) / 1024;
    
    current_directory->virt_tables[table_num]->pages[page_num] = 0x0;

    asm volatile ("invlpg (%0)" : : "a" (va));
}

void page_fault_handler(registers_t *regs)
{
    uint32_t cr2;
    asm volatile ("mov %%cr2, %0": "=r" (cr2));
    printk("Page fault at %x, faulting address %x\n", regs->eip, cr2);
    printk("Error code %x\n", regs->err_code);
    panic("Core dumb.\n", __LINE__, __FILE__);
}
