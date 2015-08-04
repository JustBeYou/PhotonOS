#ifndef _init_h
#define _init_h

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gdt.h>
#include <idt.h>
#include <pit.h>
#include <vga.h>
#include <handlers.h>
#include <io.h>
#include <system.h>
#include <keyboard.h>
#include <time.h>
#include <kheap.h>
#include <paging.h>
#include <system.h>
#include <multiboot.h>

#define FREQ 100
uint8_t *inbuffer;
char *outbuffer;

/**
  * Initialize standard i/o.
  */
void init_stdio();

/**
  * Initialize all system.
  */
void init(multiboot *mboot_ptr, uint32_t init_stack);

/**
  * Print welcome screen.
  */
void welcome();

/**
  * Log in to system.
  */
void login();

#endif
