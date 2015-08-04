#include <tty.h>
#include <vga.h>
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <io.h>

void tty_init()
{
	// basic variables of tty
	tty_row = 0;
	tty_column = 0;
	tty_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	stdout = (uint16_t*) 0xB8000;
	tabstop = 4;
	tty_clear();
}

void tty_clear()
{
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			((uint16_t*) stdout)[index] = make_vgaentry(' ', tty_color);
		}
	}
	tty_row = 0;
	tty_column = 0;
}

void tty_scroll()
{
	uint16_t blank = make_vgaentry(' ', tty_color);
	
    if (tty_row >= 25)
    {
        int temp = tty_row - 25 + 1;
        memcpy ((uint16_t*) stdout, (uint16_t*) stdout + temp * 80, (25 - temp) * 80 * 2);
        
        memsetw ((uint16_t*) stdout + (25 - temp) * 80, blank, 80);
        tty_row = 25 - 1;
    }
}

void tty_setcolor(uint8_t color)
{
	tty_color = color;
}

void tty_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	((uint16_t*) stdout)[index] = make_vgaentry(c, color);
}

void tty_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		putchar(data[i]);
}

void tty_set_tab(int size)
{
	tabstop = size;
}

void tty_move_cursor()
{
	uint16_t cursor_pos = tty_row * 80 + tty_column;
	outb(0x3D4, 14);
	outb(0x3D5, cursor_pos >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cursor_pos);
}

void clear_line(int line)
{
	tty_row = line;
	tty_column = 0;
	for (int i = 0; i <= 80; i++) {
		putchar(' ');
	}
	tty_column = 0;
	tty_row = line;
	tty_move_cursor();
}

void putchar_color(char c, enum vga_color fg)
{
	uint8_t temp_color = tty_color;
	tty_color = make_color(fg, COLOR_BLACK);
	putchar(c);
	tty_color = temp_color;
}

void wstr_color(const char* data, enum vga_color fg)
{
	for ( size_t i = 0, n = strlen(data); i < n; i++ )
		putchar_color((int) ((const unsigned char*) data)[i], fg);
}
