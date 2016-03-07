/* Programmable Interval Timer driver header
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * Contains functions declarations for PIT driver.
 * The PIT is the device that allows us to create a
 * timer at a specified frequency. PIT interrupt is IRQ 0.
 */

#ifndef _clock_h
#define _clock_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <i386/handlers.h>

/**
  * Function called by PIT handler.
  */
void timer_callback (registers_t *regs);

/**
  * Initialize the PIT.
  */
void init_timer(uint32_t freq);

/**
  * Get current tick of system.
  */
int get_tick();

#endif
