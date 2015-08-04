#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

int printf(const char* restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);
 
	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;
 
	while ( *format != '\0' ) {
		if ( *format != '%' ) {
		print_c:
			amount = 1;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			write(format, amount);
			format += amount;
			written += amount;
			continue;
		}
 
		const char* format_begun_at = format;
 
		if ( *(++format) == '%' )
			goto print_c;
 
		if ( rejected_bad_specifier ) {
		incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		} 
		if ( *format == 'c' ) {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			write(&c, sizeof(c));
		} else if ( *format == 's' ) {
			format++;
			const char* s = va_arg(parameters, const char*);
			call(1, (uint32_t) s, strlen(s), 0, 0, 0);
		} else if ( *format == 'd' ) {
			format++;
			int n = va_arg(parameters, int);
			if (n) {
				char s[intlen(n, 10)];
				itoa(s, n, 10);
				call(1, (uint32_t) s, strlen(s), 0, 0, 0);
			} else {
				printf("0");
			}
		} else if ( *format == 'x') {
			format++;
			int n = va_arg(parameters, int);
			if (n) {
				char s[intlen(n, 16)];
				itoa(s, n, 16);
				call(1, (uint32_t) s, strlen(s), 0, 0, 0);
			} else {
				printf("0x0");
			}
		} else {
			goto incomprehensible_conversion;
		}
	}
 
	va_end(parameters);
 
	return written;
}
