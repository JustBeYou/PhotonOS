/* Paging driver
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <pmm.h>
#include <stdint.h>
#include <stddef.h>
#include <io.h>
#include <string.h>
#include <system.h>
#include <kheap.h>

uint32_t nframes;
uint32_t *frames;
uint32_t placement_addr = (uint32_t) &kernel_end;
int paging_enabled = 0;

void init_pmm(uint32_t mem_size)
{
    nframes = (mem_size * 1024) / FRAME_SIZE;
    frames = kmalloc(sizeof(uint32_t) * nframes, 0, 0);
    for (uint32_t i = 0; i < nframes; i++) {
        free_frame(FRAME_ADDR_FROM_INDEX(i));
    }
    printk("Number of frames: %d and memory size: %d\n", nframes, mem_size / 1024);
}

uint32_t find_frame()
{
   for (uint32_t i = 0; i < nframes; i++) {
        if (!frames[i]) {
	    uint32_t frame_addr = FRAME_ADDR_FROM_INDEX(i);
            use_frame(frame_addr);
	    return frame_addr;
	}
   }
   printk("[ERROR] No free frames!");
   keep_running();
}

void use_frame(uint32_t addr)
{
    int frame_index = FRAME_INDEX(addr);
    frames[frame_index] = 1;
}

void free_frame(uint32_t addr)
{
    int frame_index = FRAME_INDEX(addr);
    frames[frame_index] = 0;
}
