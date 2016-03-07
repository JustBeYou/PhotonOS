#ifndef _time_h
#define _time_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

/**
  * Init clock to 0:0:0:0. 
  */
void init_time();

/**
  * Update clock. (at every tick)
  */
void update_time();

/**
  * Print current time.
  */
void print_time();

#endif
