#ifndef _stdlib_h
#define _stdlib_h

#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

#define ASSERT(b) ((#b) ? (void)0 : panic("Assertion failed!", __LINE__, __FILE__))

__attribute__((__noreturn__))
void abort(void);
__attribute__((__noreturn__))
void exit(int);

/**
  * Converts an integer to a string.
  */
void itoa(char *buf, unsigned long int n, int base);

/**
  * Get length of a integer of any base.
  */
int intlen(int n, int base);

/**
  * Converts a string to an integer.
  */
int atoi(char *str);

/**
 * Allocate n bytes of memory.
 */
void *malloc(size_t n);
void *realloc(void *p, size_t size);

/**
 * Free allocated memory.
 */
void free(void *p);

#endif
