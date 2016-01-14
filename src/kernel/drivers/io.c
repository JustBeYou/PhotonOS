/* Basic input/output
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

/* output byte */
void outb(uint32_t ad, uint8_t v)
{
    asm volatile("outb %%al, %%dx" :: "d" (ad), "a" (v));;
}
/* output word */
void outw(uint32_t ad, uint16_t v)
{
    asm volatile("outw %%ax, %%dx" :: "d" (ad), "a" (v));
}
/* output word */
void outl(uint32_t ad, uint32_t v)
{
    asm volatile("outl %%eax, %%dx" : : "d" (ad), "a" (v));
}
/* input byte */
uint8_t inb(uint32_t ad)
{
    uint8_t _v;       
    asm volatile("inb %%dx, %%al" : "=a" (_v) : "d" (ad)); 
    return _v;
}
/* input word */
uint16_t inw(uint32_t ad)
{
    uint16_t _v;         
    asm volatile("inw %%dx, %%ax" : "=a" (_v) : "d" (ad));  
    return _v;
}
/* input word */
uint32_t inl(uint32_t ad)
{
    uint32_t _v;         
    asm volatile("inl %%dx, %%eax" : "=a" (_v) : "d" (ad)); 
    return _v;
}

void print_regs(registers_t *regs)
{
    printk("\nds: %d\tedi: %d\tesi: %d\n", regs->ds, regs->edi, regs->esi);
    printk("ebp: %d\tesp: %d\tebx: %d\n", regs->ebp, regs->esp, regs->ebx);
    printk("edx: %d\tecx: %d\teax: %d\n", regs->edx, regs->ecx, regs->eax);
    printk("eip: %d\tcs: %d\teflags: %d\n", regs->eip, regs->cs, regs->eflags);
    printk("useresp: %d\tss: %d\n", regs->useresp, regs->ss);
    printk("int_no: %d\terr_code: %d\n\n", regs->int_no, regs->err_code);
}

int write_char(const char c)
{
    // TODO: implement write_char using FILE
    vga_putchar(c); 
    return 0;
}

int write(const char *buf, size_t len)
{
    for ( size_t i = 0; i < len; i++ )
        write_char((int) ((const char*) buf)[i]);
    return 0;
}

int printk(const char* format, ...)
{
    va_list parameters;
    va_start(parameters, format);
 
    int written = 0;
    size_t amount;
    bool rejected_bad_specifier = false;
 
    while ( *format != '\0' ) {
        if ( *format != '%' ) {
        print_c:
            amount = 1;
            while ( format[amount] && format[amount] != '%' )
                amount++;
            write(format, amount);
            format += amount;
            written += amount;
            continue;
        }
 
        const char* format_begun_at = format;
 
        if ( *(++format) == '%' )
            goto print_c;
 
        if ( rejected_bad_specifier ) {
        incomprehensible_conversion:
            rejected_bad_specifier = true;
            format = format_begun_at;
            goto print_c;
        } 
        if ( *format == 'c' ) {
            format++;
            char c = (char) va_arg(parameters, int /* char promotes to int */);
            write(&c, sizeof(c));
        } else if ( *format == 's' ) {
            format++;
            const char* s = va_arg(parameters, const char*);
            write(s, strlen(s));
        } else if ( *format == 'd' ) {
            format++;
            int n = va_arg(parameters, int);
            if (n) {
                char s[intlen(n, 10)];
                itoa(s, n, 10);
                write(s, strlen(s));
            } else {
                putchar('0');
            }
        } else if ( *format == 'x') {
            format++;
            int n = va_arg(parameters, int);
            if (n) {
                char s[intlen(n, 16)];
                itoa(s, n, 16);
                write(s, strlen(s));
            } else {
                printk("0x0");
            }
        } else {
            goto incomprehensible_conversion;
        }
    }
 
    va_end(parameters);
    return written;
}

void call(int32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    asm volatile (" \
    movl %0, %%eax; \
    movl %1, %%ebx; \
    movl %2, %%ecx; \
    movl %3, %%edx; \
    movl %4, %%esi; \
    movl %5, %%edi; \
    int $0x80; \
    " : : "r" (eax) , "r" (ebx) , "r" (ecx) , "r" (edx) , "r" (esi) , "r" (edi));
}
