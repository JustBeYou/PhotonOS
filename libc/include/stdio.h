#ifndef _stdio_h
#define _stdio_h

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#define STDIO_SIZE 4096

void* stdout;
void* stdin;
volatile int in_size;
int out_crs;

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
