#ifndef _process_h
#define _process_h

#include <system.h>
#include <fs/vfs.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#define PROC_USER_STACK 0x200000 // 2 MiB
#define PROC_KERN_STACK 0x80000  // 512 KiB
#define PROC_USER_HEAP  0x80000  // 512 Kib (initial size, can be expanded)

typedef struct process {
    size_t pid; // ID
    size_t esp; // current kernel stack position
    size_t ss; // current kernel stack segment
    size_t kern_stack; // where kernel stack is allocated
    size_t user_stack; // where user stack is allocated
    size_t cr3; // current page directory
    size_t time_to_run; // time allocated for process
    size_t ready; // ready or not?
    size_t prior; // priority
    char name[32]; // name of process
    
    struct file **opened_files;
    int file_table_size;
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
