#ifndef _vga_h
#define _vga_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

extern uint16_t *vga_memory;

enum vga_color
{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15,
};

/**
  * Create a color.
  */
uint8_t make_color(enum vga_color fg, enum vga_color bg);

/**
  * Create a entry for vga buffer.
  */
uint16_t make_vgaentry(char c, uint8_t color);

/**
  * Flush the vga memory from current tty buffer.
  */
void flush_vga();

/**
  * Initialize vga support and create a main tty.
  */
void init_vga();

/**
  * Clear monitor screen.
  */
void clear_vga();

/**
  * Scroll the terminal.
  */
int vga_scroll(size_t *row);

/**
  * Sets the foreground and background color of tty.
  */
void vga_setcolor(enum vga_color fg, enum vga_color bg);

/**
  * Create a char entry at 'x', 'y' coordonates with specified color.
  */
void vga_putentryat(char c, uint8_t color, size_t x, size_t y);

/**
  * Write a char in frame buffer.
  */
void vga_putchar(char c);

/**
  * Write a string on the screen.
  */
void vga_writestring(const char* data);

/**
  * Change size of a '\t' (tab) char.
  */
void vga_set_tab(int size);

/**
  * Move system cursor.
  */
void vga_move_cursor(int row, int column);

/**
  * Basic function for write colored char.
  */
void putchar_color(char c, enum vga_color fg);

/**
  * Basic function for write colored str.
  */
void wstr_color(const char* data, enum vga_color fg);

#endif
