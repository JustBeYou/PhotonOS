#ifndef _init_h
#define _init_h

#include <tty.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gdt.h>
#include <idt.h>
#include <pit.h>
#include <vga.h>
#include <isr.h>
#include <io.h>
#include <system.h>
#include <keyboard.h>
#include <time.h>
#include <kheap.h>
#include <paging.h>
#include <system.h>

#define FREQ 100
uint8_t inbuffer[STDIN_SIZE];

/**
  * Initialize standard i/o.
  */
void init_stdio();

/**
  * Initialize all system.
  */
void init();

/**
  * Print welcome screen.
  */
void welcome();

/**
  * Log in to system.
  */
void login();

#endif
