#ifndef _stdlib_h
#define _stdlib_h

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <system.h>

#define ASSERT(b) ((#b) ? (void)0 : panic("Assertion failed!", __LINE__, __FILE__))

__attribute__((__noreturn__))
void abort(void);
__attribute__((__noreturn__))
void exit(int);

/**
  * Converts a integer to a string.
  */
void itoa(char *buf, unsigned long int n, int base);

/**
  * Get length of a integer of any base.
  */
int intlen(int n, int base);

#endif
