#include <bitmap.h>
#include <stdint.h>

void use_bit(uint32_t *bitmap, uint32_t bit)
{
    bitmap[BIT_INDEX(bit)] |= (0x1 << BIT_OFFSET(bit));
}

void free_bit(uint32_t *bitmap, uint32_t bit)
{
    bitmap[BIT_INDEX(bit)] &= ~(0x1 << BIT_OFFSET(bit)); 
}

uint32_t test_bit(uint32_t *bitmap, uint32_t bit)
{
    return !(bitmap[BIT_INDEX(bit)] & (0x1 << BIT_OFFSET(bit)));
}
