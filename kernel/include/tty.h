#ifndef _tty_h
#define _tty_h

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <vga.h>
#include <system.h>

size_t tty_row;
size_t tty_column;
uint8_t tty_color;
int tabstop; 

/**
  * Init. the terminal. Set the video buffer, color the screen
  * and set text font.
  */
void tty_init();

/**
  * Clear the screen.
  */
void tty_clear();

/**
  * Scroll the terminal.
  */
void tty_scroll();

/**
  * Sets the foreground and background color of tty.
  */
void tty_setcolor(uint8_t color);

/**
  * Create a char entry at 'x', 'y' coordonates with specified color.
  */
void tty_putentryat(char c, uint8_t color, size_t x, size_t y);

/**
  * Write a string on the screen.
  */
void tty_writestring(const char* data);

/**
  * Change size of a '\t' (tab) char.
  */
void tty_set_tab(int size);

/**
  * Move system cursor.
  */
void tty_move_cursor();

/**
  * Clear line and set cursor to it.
  */
void clear_line(int line);

/**
  * Basic function for write colored char.
  */
void putchar_color(char c, enum vga_color fg);

/**
  * Basic function for write colored str.
  */
void wstr_color(const char* data, enum vga_color fg);

#endif
