/* Paging driver
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

uint32_t phys_ram_bytes;
uint32_t phys_ram_kb;
uint32_t phys_ram_mb;
uint32_t phys_ram_gb;
uint32_t placement_addr = (uint32_t) &kernel_end;
int paging_enabled = 0;

#ifdef BITMAP_FRAME_ALLOCATOR

#include <bitmap.h>

uint32_t nframes;
uint32_t *frames;
uint32_t free_frames;

void init_pmm(uint32_t mem_size)
{
    if (mem_size * 1024 < (uint32_t) MIN_MEM_NEED) {
        printk("Memory size: %d Needed: %d\n", mem_size, MIN_MEM_NEED);
        panic("[ERROR] You haven't enough memory.",
                __LINE__, __FILE__);
    }

    phys_ram_gb = mem_size / 1024 / 1024;
    phys_ram_mb = mem_size / 1024;
    phys_ram_kb = mem_size;
    phys_ram_bytes = mem_size * 1024;

    nframes = (mem_size * 1024) / FRAME_SIZE;
    frames = kmalloc(sizeof(uint32_t) * nframes / 8 +
                     sizeof(uint32_t) * nframes % 8,
                     0,
                     0);
    free_frames = nframes;

    for (uint32_t i = 0; i < nframes; i++) {
        free_bit(frames, i);
    }
}

uint32_t find_frame()
{
    for (uint32_t i = 0; i < nframes; i++) {
        if (test_bit(frames, i)) {
            return i;
        }
    }
    return -1;
}

uint32_t alloc_frame()
{
    int frame_index = find_frame();
    if (frame_index != -1) {
        use_bit(frames, frame_index);
        free_frames--;
        return frame_index * FRAME_SIZE;
    }

    panic("No free frames!", __LINE__, __FILE__);
    return -1;
}

void free_frame(uint32_t address)
{
    uint32_t frame_index = address / FRAME_SIZE;
    free_bit(frames, frame_index);
    free_frames++;
}

#endif
