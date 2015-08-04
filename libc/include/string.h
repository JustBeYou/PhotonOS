#ifndef _string_h
#define _string_h

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

/**
  * Calculate lenght of a string.
  */
size_t strlen(const char* str);

/**
  * Reverse a string.
  */
char *strrev(char *string);

/**
  * Fill a block of memory.
  */
void* memset(void* bufptr, int value, size_t size);

/**
  * Fill a block of memory using a loop.
  */
void *memsetw(void *s, int c, size_t n);

/**
  * Copy a block of memory to another location.
  */
void* memcpy(void* dstptr, const void* srcptr, size_t size);

/**
  * Compare two strings.
  */
int strcmp(const char *s1, const char *s2);

/**
  * Compare two blocks of memory.
  */
int memcmp(const void*, const void*, size_t);

/**
  * Move a block a memory to specified destination.
  */
void* memmove(void*, const void*, size_t);

#endif
