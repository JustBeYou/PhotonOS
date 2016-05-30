/* Keyboard driver
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

#include <drivers/keyboard.h>
#include <kernel/io.h>
#include <i386/handlers.h>

void (*keyboard_handler)(uint8_t *buf, uint16_t size);

// Keyboard map
char USasciiNonShift[] = {
0, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE,
TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ENTER, 0,
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0,
KF1, KF2, KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10, 0, 0,
KHOME, KUP, KPGUP,'-', KLEFT, '5', KRIGHT, '+', KEND, KDOWN, KPGDN, KINS, KDEL, 0, 0, 0, KF11, KF12 };

// Shifted scancodes to ASCII:
char USasciiShift[] = {
0, ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', BACKSPACE,
TAB, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', ENTER, 0,
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|',
'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0,
KF1, KF2, KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10, 0, 0,
KHOME, KUP, KPGUP, '-', KLEFT, '5', KRIGHT, '+', KEND, KDOWN, KPGDN, KINS, KDEL, 0, 0, 0, KF11, KF12 };

uint8_t kb_buffer[KEYBOARD_BUFFER_SIZE];
int kb_buf_pos;
int shift;
char *ascii_s;
char *ascii_S;

void install_keyboard()
{
    ascii_s = USasciiNonShift;
    ascii_S = USasciiShift;
    kb_buf_pos = 0;
    memset(kb_buffer, 0, KEYBOARD_BUFFER_SIZE);

    register_interrupt_handler(IRQ1, &keyboard_interrupt_handler);
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
            kb_buffer[kb_buf_pos++] = ascii_S[scancode];
        } else {
            kb_buffer[kb_buf_pos++] = ascii_s[scancode];
        }

        if (special) {
            uint8_t key = kb_buffer[--kb_buf_pos];
            kb_buffer[kb_buf_pos] = 0;

            // something useless to remove the 'unused' warn
            key -= key;
            key += key;
            // do special key work
        }

        if (kb_buf_pos == KEYBOARD_BUFFER_SIZE) {
            memset(kb_buffer, 0, KEYBOARD_BUFFER_SIZE);
            kb_buf_pos = 0;
        }
    }
}

char kb_getchar()
{
    volatile int pos_now = kb_buf_pos;
    while (1) {
        if (pos_now != kb_buf_pos && kb_buffer[kb_buf_pos - 1])
            break;
    }

    return (char) kb_buffer[kb_buf_pos - 1];
}
