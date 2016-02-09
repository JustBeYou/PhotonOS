#ifndef _io_h
#define _io_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

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

/* read char from stdin */
char read_char();

/* write string to stdout */
int write(const char *buf, size_t len);

/* read string from stdin */
int read(char *buf, size_t len);

/* formatted printing for kernel mode */
int printk(const char* format, ...);

/**
  * Allow C code to call system.
  */
#define syscall(eax, ebx, ecx, edx, esi, edi) \
        asm volatile ("movl %0, %%eax;" \
        "movl %1, %%ebx;" \
        "movl %2, %%ecx;" \
        "movl %3, %%edx;" \
        "movl %4, %%esi;" \
        "movl %5, %%edi;" \
        "int $0x80;" \
        : : "r" (eax) , "r" (ebx) , "r" (ecx) , "r" (edx) , "r" (esi) , "r" (edi))

#endif
