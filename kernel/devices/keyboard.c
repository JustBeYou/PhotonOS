#include <keyboard.h>
#include <isr.h>
#include <system.h>
#include <io.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

                                           /* USA keyboard. */
// Non-Shifted scancodes to ASCII:
static char USasciiNonShift[] = {
0, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE,
TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ENTER, 0,
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0,
KF1, KF2, KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10, 0, 0,
KHOME, KUP, KPGUP,'-', KLEFT, '5', KRIGHT, '+', KEND, KDOWN, KPGDN, KINS, KDEL, 0, 0, 0, KF11, KF12 };

// Shifted scancodes to ASCII:
static char USasciiShift[] = {
0, ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', BACKSPACE,
TAB, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', ENTER, 0,
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|',
'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0,
KF1, KF2, KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10, 0, 0,
KHOME, KUP, KPGUP, '-', KLEFT, '5', KRIGHT, '+', KEND, KDOWN, KPGDN, KINS, KDEL, 0, 0, 0, KF11, KF12 };

void install_keyboard()
{
	in_size = 0;
	
	ascii_s = USasciiNonShift;
	ascii_S = USasciiShift;
	last = 0;
	
	keyboard_set_handler(&read_kb_buff);
	register_interrupt_handler(IRQ1, &keyboard_interrupt_handler);
}

void keyboard_set_handler(void (*callback)(uint8_t *buf, uint16_t size))
{
	keyboard_handler = callback;
}

void keyboard_interrupt_handler(__attribute__ ((unused)) registers_t *regs)
{
	uint8_t scancode = inb(0x60);
	int special = 0;
	
	if (scancode & 0x80) {
		scancode &= 0x7F;
		if (scancode == KRLEFT_SHIFT || scancode == KRRIGHT_SHIFT) {
			shift = 0;
			special = 1;
		}
	} else {
		if (scancode == KRLEFT_SHIFT || scancode == KRRIGHT_SHIFT) {
			shift = 1;
			special = 1;
		}
		
		if (shift) {
			kb_buffer[last++] = ascii_S[scancode];
		} else {
			kb_buffer[last++] = ascii_s[scancode];
		}
		
		if (special != 1) {
			cli();
			keyboard_handler(kb_buffer, last);
			sti();
		}
		
		if (last == KEYBOARD_BUFFER_SIZE) {
			last = 0;
		}
	}
}

void read_kb_buff(uint8_t *buf, uint16_t size) 
{
	((uint8_t*) stdin)[in_size] = buf[size - 1];
}
