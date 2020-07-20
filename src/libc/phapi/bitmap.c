#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

void use_bit(size_t *bitmap, size_t bit)
{
    bitmap[BIT_INDEX(bit)] |= (0x1 << BIT_OFFSET(bit));
}

void free_bit(size_t *bitmap, size_t bit)
{
    bitmap[BIT_INDEX(bit)] &= ~(0x1 << BIT_OFFSET(bit)); 
}

size_t test_bit(size_t *bitmap, size_t bit)
{
    return !(bitmap[BIT_INDEX(bit)] & (0x1 << BIT_OFFSET(bit)));
}
