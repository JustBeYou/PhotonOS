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

/* write char to vga */
int vga_write_char(const char c);

/* read char from keyboard */
char kb_read_char();

/* write string to vga */
int vga_write(const char *buf, size_t len);

/* read string from keyboard */
int kb_read(char *buf, size_t len);

/* get char from keyboard */
int getchark();

/* get line from keyboard */
char *getsk(char *str);

/* put string to vga */
int putsk(const char* string);

/* formatted printing for kernel mode */
int printk(const char* format, ...);

// Kernel space file functions
int kopen(const char *pathname, int flags);

size_t kread(int fd, void *buf, size_t count);

int kclose(int fd);

// User space file functions
int open(const char *pathname, int flags);

int read(int fd, char *buf, size_t count);

int write(int fd, char *buf, size_t count);

int close(int fd);

#endif
