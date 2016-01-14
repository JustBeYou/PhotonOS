#ifndef _stdio_h
#define _stdio_h

#include <system.h>
#include <stdlib.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

#define STDIO_SIZE 4096

struct __sbuf {
    unsigned char *_base;
    int _size;
};

typedef struct __sFILE {
    unsigned char *_p;
    int _r;
    int _w;
    short _flags;
    short _file;
    struct __sbuf _bf;
    
    int (*_close)(void *);
    int (*_read)(void *, char *, int);
    // TODO: add _seek
    int (*_write)(void *, const char *, int);
    
    size_t _offset;
} FILE;

/**
  * Puts a char in standard out buffer.
  */
void putchar(char c);

/**
  * Write a string in standard out buffer using formatting.
  */
int printf(const char* format, ...);

/**
  * Write a string in standard out buffer and adds NEWLINE at end. 
  */
int puts(const char*);

/**
  * Gets a char from standard input and print it.
  */
int getchar();

/**
  * Gets a char from standard input and don't print it.
  */
int getch();

/**
  * Get a string from standard input until NEWLINE char is detected.
  */
char *gets(char *str);

#endif
