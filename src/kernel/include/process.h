#ifndef _process_h
#define _process_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

typedef struct process {
    uint32_t pid;
    uint32_t esp;
    uint32_t cr3;
    uint32_t time_to_run;
    uint8_t ready;
    uint8_t prior;
    char name[32];
    struct process *next;
} process_t;

/**
  * Init user mode.
  */
void init_usermode();

/**
  * System calls handler.
  */
void syscall_handler(registers_t *regs);

/**
  * Initialize processing.
  */
void init_multitasking();

/**
  * Create a new process.
  */
process_t *create_process(char *name);

/**
  * Switch the process.
  */
void switch_process();

extern int read_eip();

#endif