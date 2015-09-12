#ifndef _bitmap_h
#define _bitmap_h

#include <stdint.h>

#define BIT_INDEX(x) (x / 0x20)
#define BIT_OFFSET(x) (x % 0x20)

void use_bit(uint32_t *bitmap, uint32_t bit);
void free_bit(uint32_t *bitmap, uint32_t bit);
uint32_t test_bit(uint32_t *bitmap, uint32_t bit);

#endif
