#ifndef _task_h
#define _task_h

#include <io.h>

typedef struct task {
	uint32_t pid;
	t_regs_t regs;
	struct task *next;
} task_t;

task_t *current_task;
task_t *start_task;
uint32_t pid;

/**
  * Init user mode. 
  */
void init_usermode();

/**
  * System calls handler.
  */
void syscall_handler(registers_t *regs);

/**
  * Initialize tasking.
  */
void init_tasking();

/**
  * Create a new task.
  */
task_t *create_task(task_t *new_task, void (*main)(), int32_t flags, uint32_t pagedir);

/**
  * Switch task frontend.
  */
void preempt();

/**
  * Switch the task.
  */
void switch_task();

extern int read_eip();

#endif
