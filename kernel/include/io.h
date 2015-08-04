#ifndef _io_h
#define _io_h

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <handlers.h>
#include <system.h>

/* output byte */
void outb(uint32_t ad, uint8_t v);

/* output word */
void outw(uint32_t ad, uint16_t v);

/* output word */
void outl(uint32_t ad, uint32_t v);

/* input byte */
uint8_t inb(uint32_t ad);

/* input word */
uint16_t inw(uint32_t ad);

/* input word */
uint32_t inl(uint32_t ad);

/* print registers */
void print_regs(registers_t *regs);

/* write char to stdout */
int write_char(const char c);

/* write string to stdout */
int write(const char *buf, size_t len);

/* formatted printing for kernel mode */
int printk(const char* format, ...);

/**
  * Allow C code to call system.
  */
void call(int32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);

#endif
