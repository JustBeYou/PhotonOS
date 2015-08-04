#include <stdio.h>
#include <tty.h>
#include <vga.h>

void putchar(char c)
{	
	switch (c) {
		case '\n':
			++tty_row;
			tty_column = -1;
			break;

		case '\t':
			for (int i = 0; i < tabstop; ++i) {
				putchar(' ');
			}
			break;

		case '\b':
			--tty_column;
			tty_putentryat(' ', tty_color, tty_column, tty_row);
			--tty_column;
			break;

		case '\a':
			tty_writestring("\n[SYSTEM]: No support for '\\a' = sound char!\n");
			// no sound support yet
			break;

		default:
			tty_putentryat(c, tty_color, tty_column, tty_row);
			break;
	}
	tty_column++;
	if (tty_column == 80) { 
		tty_row++;
		tty_column = 0; 
	}
	tty_scroll();
	tty_move_cursor();
}
