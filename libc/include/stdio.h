#ifndef _stdio_h
#define _stdio_h

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#define STDIN_SIZE 4096

void* stdout;
void* stdin;
volatile int in_size;

/**
  * Puts a char on the screen.
  */
void putchar(char c);

/**
  * Write a string on the screen using formatting.
  */
int printf(const char* restrict format, ...);

/**
  * Gets a char from keyboard and print it.
  */
int getchar();

/**
  * Gets a char from keyboard and don't print it.
  */
int getch();

/**
  * Get a string from keyboard until NEWLINE char is detected.
  */
char *gets(char *str);

#endif
