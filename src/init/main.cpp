/* All starts here!
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <kernel/kernel_class.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <kernel/heap.h>
#include <kernel/multiboot.h>
#include <kernel/process.h>
#include <kernel/shell.h>
#include <kernel/time.h>
#include <kernel/vga.h>
#include <kernel/io.h>
#include <kernel/ui.h>
#include <kernel/syscalls.h>
#include <i386/pmm.h>
#include <i386/vmm.h>
#include <i386/pit.h>
#include <i386/handlers.h>
#include <i386/gdt.h>
#include <i386/idt.h>
#include <i386/pic.h>
#include <drivers/keyboard.h>
#include <fs/vfs.h>
#include <fs/initrd.h>

#define TIMER_FREQ 100

extern int start_tasking;

extern size_t kernel_init_stack;
extern multiboot *kernel_mboot;

extern size_t phys_ram_mb;
extern size_t placement_addr;

extern size_t nframes;
extern size_t init_esp;
extern size_t kernel_init_stack;
extern multiboot *kernel_mboot;
extern mem_heap_t *kernel_heap;

extern char user[20];
extern char machine[30];

size_t user_stack;
size_t initrd_start;
size_t initrd_end;

void kernel_init(multiboot *mboot_ptr, size_t init_stack)
{
    kernel_init_stack = init_stack;
    kernel_mboot = mboot_ptr;
    initrd_start = *((size_t*) mboot_ptr->mods_addr);
    initrd_end = *(size_t*) (mboot_ptr->mods_addr + 4);
    placement_addr = initrd_end;

    cli();
    init_esp = kernel_init_stack;

    init_vga();

    printk("%s %s (%s) by %s. Copyright C 2016 %s. All rights reserved.\n", OS_Name, Version, Relase_Date, Author, Author);
    detect_cpu();
    printk("\n-------------------------------------------------------------------\n");

    printk("VGA driver was installed! ");
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Initialize GDT.   ");
    init_gdt();
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Initialize IDT and interrupts.   ");
    init_idt();
    wstr_color("[OK]\n", COLOR_GREEN);


    // mask some interrupts
    IRQ_set_mask(2);
    IRQ_set_mask(3);
    IRQ_set_mask(4);
    IRQ_set_mask(5);
    IRQ_set_mask(7);
    IRQ_set_mask(8);
    IRQ_set_mask(9);
    IRQ_set_mask(10);
    IRQ_set_mask(11);
    IRQ_set_mask(12);
    IRQ_set_mask(13);
    IRQ_set_mask(14);
    IRQ_set_mask(15);

    printk("Install timer and clock.   ");
    init_timer(TIMER_FREQ);
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Install keyboard support.   ");
    install_keyboard();
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Enable A20.    ");
    enable_A20();
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Initialize PMM and VMM.   ");
    init_pmm(kernel_mboot->mem_lower + kernel_mboot->mem_upper);
    init_vmm();
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Total system memory: %d MiB in %d frames.\n", phys_ram_mb,
           nframes);

    printk("Initialize kernel heap.    ");
    init_heap(DEFAULT_HEAP_SIZE);
    wstr_color("[OK]\n", COLOR_GREEN);
    printk("Initialized kernel heap at %x and created main block of %d bytes.\n",
            (size_t) kernel_heap + MEM_HEADER_SIZE, kernel_heap->mem_size);

    KernelClass mainKernelClass;
    mainKernelClass.setVersion(Version);
    mainKernelClass.setID(104);
    printk("Testing C++. Kernel Class: %s %d.    ", mainKernelClass.getVersion(), mainKernelClass.getID());
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Loading initrd.    ");
    initrd_load(initrd_start);
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Install VFS.    ");
    init_vfs();
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Initialize task switching.    ");
    init_multitasking();
    wstr_color("[OK]\n", COLOR_GREEN);

    wstr_color("\nDONE!", COLOR_GREEN);
    jmp_to_usermode();
}

extern int switch_on;

void kernel_main()
{
    switch_on = 1;
    welcome();
    login();
    prompt();
}

#ifdef __cplusplus
}
#endif
