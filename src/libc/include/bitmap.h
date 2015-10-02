#ifndef _bitmap_h
#define _bitmap_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

#define BIT_INDEX(x) (x / 0x20)
#define BIT_OFFSET(x) (x % 0x20)

/**
 * Set bit as used.
 */
void use_bit(uint32_t *bitmap, uint32_t bit);

/**
 * Set bit as free.
 */
void free_bit(uint32_t *bitmap, uint32_t bit);

/**
 * Test if bit is free or not.
 */
uint32_t test_bit(uint32_t *bitmap, uint32_t bit);

#endif
