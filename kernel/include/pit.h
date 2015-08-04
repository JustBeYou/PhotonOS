#ifndef _clock_h
#define _clock_h

#include <stdint.h>
#include <stddef.h>
#include <isr.h>
#include <system.h>

/**
  * Function called by PIT handler.
  */
void timer_callback (__attribute__ ((unused)) registers_t *regs);

/**
  * Initialize the PIT.
  */
void init_timer(uint32_t freq);

/**
  * Get current tick of system.
  */
int get_tick();

#endif
