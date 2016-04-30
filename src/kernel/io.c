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

#include <i386/handlers.h>
#include <kernel/vga.h>
#include <kernel/io.h>
#include <kernel/process.h>
#include <drivers/keyboard.h>
#include <fs/vfs.h>

extern void *stdin;
extern volatile uint32_t in_cursor;
extern struct file_operations fops;
extern struct file **opened_files;
extern int file_table_size;
extern char kernel_space_open;
extern process_t *current_process;

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
    printk("\n\rPushed by CPU:\n\r");
    printk("ss: %x uesp: %x eflags: %x cs: %x eip: %x\n\r",
        regs->ss,
        regs->uesp,
        regs->eflags,
        regs->cs,
        regs->eip
    );
    printk("int_no: %x err_code: %x\n\r", regs->int_no, regs->err_code);
    printk("Pushed by pusha:\n\r");
    printk("eax: %x ecx: %x edx: %x ebx: %x kesp: %x\n\r",
        regs->eax,
        regs->ecx,
        regs->edx,
        regs->ebx,
        regs->kesp
    );
    printk("ebp: %x esi: %x edi: %x\n\r",
        regs->ebp,
        regs->esi,
        regs->edi
    );
    printk("Others:\n\r");
    printk("es: %x ds: %x fs: %x gs: %x\n\r",
        regs->es,
        regs->ds,
        regs->fs,
        regs->gs
    );
    printk("cr2: %x cr3: %x\n\r",
        regs->cr2,
        regs->cr3
    );
}

int vga_write_char(const char c)
{
    vga_putchar(c);
    return 0;
}

char kb_read_char ()
{
    while (true) {
        if (((uint8_t*)stdin)[in_cursor] != 0) {
            in_cursor++;
            break;
        }
    }
    int c = ((uint8_t*)stdin)[in_cursor - 1];
    return c;
}

int vga_write(const char *buf, size_t len)
{
    for ( size_t i = 0; i < len; i++ )
        vga_write_char((int) ((const char*) buf)[i]);
    return 0;
}

int kb_read(char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        buf[i] = kb_read_char();
    return 0;
}


int getchark()
{
    int c = kb_read_char();
    vga_write_char(c);

    return c;
}

char *getsk(char *str)
{
    int c = kb_read_char();
    int i = 0;
    while (c != '\n') {
        if (c != '\b') {
            str[i++] = c;
            vga_write_char(c);
        } else if (c == '\b' && i > 0) {
            str[--i] = 0;
            vga_write_char(c);
        }
        c = kb_read_char();
    }
    str[i] = '\0';
    vga_write_char('\n');
    return str;
}

int putsk(const char* string)
{
    int i = 0;
    for (i = 0; string[i] != '\0'; i++) {
        vga_write_char(string[i]);
    }
    vga_write_char('\n');
    return i;
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
            vga_write(format, amount);
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
            char c = (char) va_arg(parameters, int);
            vga_write(&c, sizeof(c));
        } else if ( *format == 's' ) {
            format++;
            const char* s = va_arg(parameters, const char*);
            vga_write(s, strlen(s));
        } else if ( *format == 'd' ) {
            format++;
            int n = va_arg(parameters, int);
            if (n) {
                char s[intlen(n, 10)];
                itoa(s, n, 10);
                vga_write(s, strlen(s));
            } else {
                vga_write_char('0');
            }
        } else if ( *format == 'x') {
            format++;
            int n = va_arg(parameters, int);
            if (n) {
                char s[intlen(n, 16)];
                itoa(s, n, 16);
                vga_write(s, strlen(s));
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

// Kernel space file functions
int kopen(const char *pathname, int flags)
{
    kernel_space_open = 1;
    
    file *f = kmalloc(sizeof(file), 0, 0);
    f->f_dentry = get_dentry_by_path(pathname);
    f->f_op = &fops;
    f->f_mode = (mode_t) flags;
    f->f_pos = 0;
    f->f_uid = 0;
    f->f_gid = 0;
    f->f_version = 0;
    f->f_dentry->inode->open_flags = flags;

    return f->f_op->open(f->f_dentry->inode, f);
}

size_t kread(int fd, void *buf, size_t count)
{
    if (fd >= file_table_size || buf == NULL) {
        return -1;
    }
    
    file *f = opened_files[fd];
    
    if (f == NULL) {
        return -1;
    }
    
    loff_t off = (loff_t) f->f_dentry->inode->offset;
    return f->f_op->read(f, buf, count, &off);
}

int kclose(int fd)
{
    if (fd >= file_table_size) {
        return -1;
    }
    
    file *f = opened_files[fd];
    opened_files[fd] = NULL;
    inode_rewind(f->f_dentry->inode);
    kfree(f);
    
    return 0;
}

// User space file functions
int open(const char *pathname, int flags)
{
    kernel_space_open = 0;
    
    file *f = kmalloc(sizeof(file), 0, 0);
    f->f_dentry = get_dentry_by_path(pathname);
    f->f_op = &fops;
    f->f_mode = (mode_t) flags;
    f->f_pos = 0;
    f->f_uid = 0;
    f->f_gid = 0;
    f->f_version = 0;
    f->f_dentry->inode->open_flags = flags;

    return f->f_op->open(f->f_dentry->inode, f);
}

int read(int fd, char *buf, size_t count)
{
    if (fd >= current_process->file_table_size || buf == NULL) {
        return -1;
    }
    
    file *f = current_process->opened_files[fd];
    
    if (f == NULL) {
        return -1;
    }
    
    loff_t off = (loff_t) f->f_dentry->inode->offset;
    return f->f_op->read(f, buf, count, &off);
}

int write(int fd, char *buf, size_t count)
{
    return -1;
}

int close(int fd)
{
    if (fd >= current_process->file_table_size) {
        return -1;
    }
    
    file *f = current_process->opened_files[fd];
    current_process->opened_files[fd] = NULL;
    inode_rewind(f->f_dentry->inode);
    kfree(f);
    
    return 0;
}
