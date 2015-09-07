/* Kernel heap driver
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <kheap.h>
#include <vmm.h>
#include <pmm.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <system.h>
#include <io.h>

extern int paging_enabled;
extern uint32_t placement_addr;

void init_heap ()
{
}

void *kmalloc(size_t size, int align, uint32_t *phys)
{
    if (paging_enabled) {
	panic("kmalloc isn't ready to work with paging.\n", __LINE__, __FILE__);
    } else {
        if (align && (placement_addr & 0xFFFFF000)) {
            placement_addr &= 0xFFFFF000;
	        placement_addr += 0x1000;
	    }

	    if (phys) {
            *phys = placement_addr;
	    }
	    uint32_t ret_addr = placement_addr;
	    placement_addr += size;
	    void *ret_p = (void*) ret_addr;
	    return ret_p;
    }
    return NULL;
}

void kfree(void *p)
{
	printk("%d", (uint32_t) p);
}
