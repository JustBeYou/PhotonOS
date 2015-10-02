#ifndef _stdio_h
#define _stdio_h

#include <system.h>
#include <stdlib.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

#define STDIO_SIZE 4096

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
