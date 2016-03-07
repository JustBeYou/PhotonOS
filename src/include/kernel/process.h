#ifndef _process_h
#define _process_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#define PROC_USER_STACK 0x200000 // 2 MiB
#define PROC_KERN_STACK 0x80000  // 512 KiB
#define PROC_USER_HEAP  0x80000  // 512 Kib (initial size, can be expanded)

typedef struct process {
    uint32_t pid; // ID
    uint32_t esp; // current kernel stack position
    uint32_t ss; // current kernel stack segment
    uint32_t kern_stack; // where kernel stack is allocated
    uint32_t user_stack; // where user stack is allocated
    uint32_t cr3; // current page directory
    uint32_t time_to_run; // time allocated for process
    uint32_t ready; // ready or not?
    uint32_t prior; // priority
    char name[32]; // name of process
    struct process *next; // next process in list
} process_t;

/**
  * Initialize processing.
  */
void init_multitasking();

/**
  * Create a new process.
  */
process_t *create_process(char *name);

/**
  * Fork the process.
  */
int fork();

extern int read_eip();

#endif
