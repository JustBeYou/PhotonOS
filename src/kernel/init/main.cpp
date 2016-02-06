/* All starts here!
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <kernel_class.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#define TIMER_FREQ 100

extern int start_tasking;

extern uint32_t kernel_init_stack;
extern multiboot *kernel_mboot;

extern uint32_t nframes;
extern uint32_t init_esp;
extern uint32_t kernel_init_stack;
extern multiboot *kernel_mboot;
extern mem_heap_t *kernel_heap;

extern char user[20];
extern char machine[30];

void* stdin;
volatile int in_cursor;

uint8_t inbuffer[STDIO_SIZE];

static uint32_t get_total_memory()
{
    // return memory in MiB
    return nframes * FRAME_SIZE / 1024 / 1024;
}

void kernel_init(multiboot *mboot_ptr, uint32_t init_stack)
{
    kernel_init_stack = init_stack;
    kernel_mboot = mboot_ptr;

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

    printk("Total system memory: %d MiB in %d frames.\n", get_total_memory(),
           nframes);

    printk("Initialize stdio (allow using of stdio header).   ");

    stdin = (uint8_t*) inbuffer;

    for (int i = 0; i < STDIO_SIZE; i++) {
        inbuffer[i] = 0;
    }
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Initialize kernel heap.    ");
    init_heap();
    wstr_color("[OK]\n", COLOR_GREEN);
    printk("Initialized kernel heap at %x and created main block of %d bytes.\n",
            (size_t) kernel_heap + MEM_HEADER_SIZE, kernel_heap->mem_size);

    KernelClass mainKernelClass;
    mainKernelClass.setVersion(Version);
    mainKernelClass.setID(104);
    printk("Testing C++. Kernel Class: %s %d.    ", mainKernelClass.getVersion(), mainKernelClass.getID());
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Initialize FDC driver.    ");
    fdc_init();
    wstr_color("[OK]\n", COLOR_GREEN);

    printk("Initialize tasking.    ");
    init_multitasking();

    uint32_t sectornum = 0;
    char sectornumbuf[4];
    uint8_t* sector = NULL;

/*
    *** REMOVED BECAUSE IS UNSTABLE CODE ***
    printk("Initialize Virtual File System.    ");
    init_vfs();
    //wstr_color("[OK]\n", COLOR_GREEN);
    wstr_color("[ERROR]\n", COLOR_RED);

    init_tasking();
    wstr_color("[OK]\n", COLOR_GREEN);
    TODO: GET STABLE WITH TASKING AND VFS
*/

    wstr_color("\nDONE!", COLOR_GREEN);

    sti();
    getch();
}

void kernel_main()
{
    welcome();
    login();
    prompt();
}

#ifdef __cplusplus
}
#endif
