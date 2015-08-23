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

void init_heap ()
{
}

void *kmalloc (uint32_t p)
{
    return 0xdeadbeef;
}

void kfree (void *p)
{
	p = 0xdeadbeef;
}
