#ifndef _time_h
#define _time_h

#include <system.h>

int mseconds;
int seconds;
int minutes;
int hours;

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
