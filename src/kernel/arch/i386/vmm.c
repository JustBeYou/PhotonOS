#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

extern uint32_t placement_addr;
extern uint32_t paging_enabled;

page_directory_t *kernel_directory;
page_directory_t *current_directory;
multiboot *kernel_mboot;
uint32_t kernel_init_stack;
uint32_t init_esp;
uint32_t mapped_pages;

static void map_area(uint32_t from_va, uint32_t to_va, uint32_t flags)
{
    for (uint32_t va = from_va; va < to_va; va += 0x1000) {
        uint32_t pa = alloc_frame();
        map(va, pa, flags);
    }
}

void init_vmm()
{
    mapped_pages = 0;

    register_interrupt_handler(14, page_fault_handler);

    kernel_directory = kmalloc(sizeof(page_directory_t), 1, 0);
    memset(kernel_directory, 0, sizeof(page_directory_t));

    for (int i = 0; i < 1024; i++) {
        kernel_directory->phys_tables[i] = 0x0 | PAGE_READ_WRITE | PAGE_USER;
    }

    set_current_directory(kernel_directory);

    // Map kernel + 16 MiB
    uint32_t mem_to_map = (size_t) &kernel_end + 0x1000000;
    map_area(0x0, mem_to_map, PAGE_READ_WRITE | PAGE_PRESENT | PAGE_USER);

    switch_page_directory(kernel_directory);
    enable_paging();
}

void set_current_directory(page_directory_t *dir)
{
    current_directory = dir;
}

void switch_page_directory(page_directory_t *dir)
{
    set_current_directory(dir);
    write_cr3(current_directory);
}

void enable_paging()
{
    uint32_t cr0 = read_cr0();
    cr0 |= 0x80000000;
    write_cr0(cr0);
    paging_enabled = 1;
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

        current_directory->phys_tables[table_num] = phys | flags;
    }
    current_directory->virt_tables[table_num]->pages[page_num] = pa | flags;
    mapped_pages++;
}

void unmap(uint32_t va)
{
    uint32_t page_num = (va / 4096) % 1024;
    uint32_t table_num = (va / 4096) / 1024;

    current_directory->virt_tables[table_num]->pages[page_num] = 0x0;

    asm volatile ("invlpg (%0)" : : "a" (va));
    mapped_pages--;
}

void page_fault_handler(registers_t *regs)
{
    uint32_t cr2 = read_cr2();
    printk("Page fault at %x, faulting address %x\n", regs->eip, cr2);
    printk("Error code %x\n", regs->err_code);
    panic("Core dumb.\n", __LINE__, __FILE__);
}
