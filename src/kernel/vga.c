/* VGA driver
 *
 * Copyright (c) 2015 Feraru Mihail (mihailferaru2000@gmail.com).
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#include <kernel/vga.h>
#include <kernel/io.h>

uint16_t *vga_memory;
#ifdef _TEXTMODE
int tabstop;
size_t row;
size_t col;
enum vga_color default_bg = COLOR_BLACK;
enum vga_color default_fg = COLOR_LIGHT_GREY;
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
#endif

extern void cmd_dbg();

#ifdef _TEXTMODE
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}
#endif

void init_vga()
{
    #ifdef _TEXTMODE
    vga_memory = (uint16_t*) 0xB8000;
    row = 0;
    col = 0;
    tabstop = 4;
    default_bg = COLOR_BLACK;
    default_fg = COLOR_LIGHT_GREY;
    clear_vga();
    #endif
}

void clear_vga()
{
    #ifdef _TEXTMODE
    for ( size_t y = 0; y < VGA_HEIGHT; y++ )
    {
        for ( size_t x = 0; x < VGA_WIDTH; x++ )
        {
            const size_t index = y * VGA_WIDTH + x;
            ((uint16_t*) vga_memory)[index] = make_vgaentry(' ', make_color(default_fg, default_bg));
        }
    }
    row = 0;
    col = 0;
    vga_move_cursor(row, col);
    #endif
}

int vga_scroll(size_t *row)
{
    #ifdef _TEXTMODE
    uint16_t blank = make_vgaentry(' ', make_color(default_fg, default_bg));

    if (*row >= 25)
    {
        int temp = *row - 25 + 1;
        memcpy ((uint16_t*) vga_memory, (uint16_t*) vga_memory + temp * 80, (25 - temp) * 80 * 2);

        memsetw ((uint16_t*) vga_memory + (25 - temp) * 80, blank, 80);
        *row = 24;
    }
    vga_move_cursor(*row, col);
    return 0;
    #endif
}

#ifdef _TEXTMODE
void vga_setcolor(enum vga_color fg, enum vga_color bg)
{
    default_bg = bg;
    default_fg = fg;
}

void vga_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    ((uint16_t*) vga_memory)[index] = make_vgaentry(c, color);
}
#endif

void vga_putchar(char c)
{
    #ifdef _TEXTMODE
    switch (c) {
        /**
         * \n  - new line
         * \t  - tab
         * \b  - backspace
         * \r  - return
         * `   - debug shell
         * ~   - halt system
         * \0  - nothing
         * \17 - wait for key
         */
        case '\n':
            row++;
            col = -1;
            break;
        case '\r':
            col = 0;
            break;
        case '\t':
            for (int i = 1; i < tabstop; i++) {
                vga_putchar(' ');
            }
            break;
        case '\b':
            vga_putentryat(' ', make_color(default_fg, default_bg), --col, row);
            --col;
            break;
        case '~':
            printk("[INFO] Halt char printed.\n STOP.");
            keep_running();
            break;
        case '`':
            cmd_dbg();
            break;
        case 0:
            break;
        case '\17':
            kb_read_char();
            break;
        default:
            vga_putentryat(c, make_color(default_fg, default_bg), col, row);
            break;
    }
    if (++col >= 80) {
        col = -1;
        ++row;
    }
    vga_scroll(&row);
    vga_move_cursor(row, col + 1);
    #endif
}

void vga_writestring(const char* data)
{
    #ifdef _TEXTMODE
    size_t datalen = strlen(data);
    for ( size_t i = 0; i < datalen; i++ )
        vga_putchar(data[i]);
    #endif
}

#ifdef _TEXTMODE
void vga_set_tab(int size)
{
    tabstop = size;
}

void vga_move_cursor(int row, int column)
{
    uint16_t cursor_pos = row * 80 + column - 1;
    outb(0x3D4, 14);
    outb(0x3D5, cursor_pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursor_pos);
}
#endif

void vga_putchar_color(char c, enum vga_color fg)
{
    #ifdef _TEXTMODE
    uint8_t temp_color = default_fg;
    default_fg = fg;
    vga_putchar(c);
    default_fg = temp_color;
    #endif
}

void wstr_color(const char* data, enum vga_color fg)
{
    #ifdef _TEXTMODE
    for ( size_t i = 0, n = strlen(data); i < n; i++ )
        vga_putchar_color((int) ((const unsigned char*) data)[i], fg);
    #endif
}

#ifndef _TEXTMODE
#error "Graphic Mode not ready!"
#endif
