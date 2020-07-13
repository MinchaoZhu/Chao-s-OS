#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

char* uint2string(unsigned int n, char* buf, size_t max_size){
	int i = max_size - 1;
	buf[i] = '\0';
	if(n == 0){
		buf[--i] = '0';
	}
	while(i>=0 && n>0){
		--i;
		buf[i] = n % 10 + '0';
		n = n / 10;
	}

	return n == 0 ? buf + i : NULL;
}

char* int2string(int n, char* buf, size_t max_size){
	if(n < 0){
		++n;
		unsigned int num = 0 - n;
		num += 1;
		char* tmp =  uint2string(num, buf, max_size - 1);
		*(--tmp) = '-';
		return tmp;
	}
	else{
		unsigned int num = n;
		return uint2string(num, buf, max_size);
	}
}


size_t hex_digits_max = 0;
char   hex_flag = 'x';
char   hex_complement = '0';
char   format_sign = '#';
char* hex2string(unsigned int n, char* buf, size_t max_size){
		char mapx[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
		char mapX[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

		int i = max_size - 1;
		size_t num = 0;
		buf[i] = '\0';
		while(i>=0 && n>0){
			--i;
			++num;
			buf[i] = hex_flag == 'x' ? mapx[n % 16] : mapX[n % 16];
			n = n >> 4;
		}
		while(num < hex_digits_max) {
			--i;
			++num;
			buf[i] = hex_complement;
		}
		if(format_sign == '#') {
			buf[--i] = 'x';
			buf[--i] = '0';
		}

		return n == 0 ? buf + i : NULL;
	
}


/**
 * current implemented format
 * %d:           signed int
 * %c:           character
 * %s:           string
 * %x %X:        unsigned hex
 * %0x %0X:       unsigned hex
 * %8x %8X:      unsigned hex extended to 8 digits by being complemented by ' '
 * %08x %08X:    unsigned hex extended to 8 digits by being complemented by '0'
 * %#x %#X:      unsigned hex
 * %#0x %#X:     unsigned hex
 * %#8x %#8X:    unsigned hex extended to 8 digits by being complemented by ' '
 * %#08x %#08X:  unsigned hex extended to 8 digits by being complemented by '0'
 * */
int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	char buf[100];
	size_t buf_size = 100;
	char* str = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			str = va_arg(parameters, const char*);
		} else if (*format == 'd') {
			format++;
			int n = (int) va_arg(parameters, int);
			str = int2string(n, buf, buf_size);
		} else if (*format == 'x' || *format == 'X') {
			hex_flag = *format;
			hex_digits_max = 0;
			format_sign = ' ';
			format++;
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
		} else if (*format == '0' && (*(format+1) == 'x' || *(format+1) == 'X')) {
			//%0x %0X
			hex_flag = *(format+1);
			hex_digits_max = 0;
			format_sign = ' ';
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
			format += 2;
		} else if (*format == '0' && *(format+1) == '8' && (*(format+2) == 'x' || *(format+2) == 'X')) {
			//%08x %08X
			hex_flag = *(format+2);
			hex_digits_max = 8;
			format_sign = ' ';
			hex_complement = '0';
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
			format += 3;

		} else if (*format == '8' && (*(format+1) == 'x' || *(format+1) == 'X')) {
			//%8x %8X
			hex_flag = *(format+1);
			hex_digits_max = 8;
			format_sign = ' ';
			hex_complement = ' ';
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
			format += 2;
		} else if (*format == '#' && (*(format+1) == 'x' || *(format+1) == 'X')) {
			// %#x %#X
			hex_flag = *(format+1);
			hex_digits_max = 0;
			format_sign = '#';
			format += 2;
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
		} else if (*format == '#' && *(format+1) == '0' && (*(format+2) == 'x' || *(format+2) == 'X')) {
			//%#8x %#8X
			hex_flag = *(format+2);
			hex_digits_max = 0;
			format_sign = '#';
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
			format += 3;
		} else if (*format == '#' && *(format+1) == '0' && *(format+2) == '8' && (*(format+3) == 'x' || *(format+3) == 'X')) {
			//#%08x %#08X
			hex_flag = *(format+3);
			hex_digits_max = 8;
			format_sign = '#';
			hex_complement = '0';
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
			format += 4;

		} else if (*format == '#' && *(format+1) == '8' && (*(format+2) == 'x' || *(format+2) == 'X')) {
			//%#8x %8X
			hex_flag = *(format+2);
			hex_digits_max = 8;
			format_sign = '#';
			hex_complement = ' ';
     		unsigned int n = (unsigned int) va_arg(parameters, unsigned int);
			str = hex2string(n, buf, buf_size);
			format += 3;
		} 
		else {
			format = format_begun_at;
			size_t len = strlen(format);
			str = format;
			format += len;
		}

		size_t len = strlen(str);
		if (maxrem < len) {
			// TODO: Set errno to EOVERFLOW.
			return -1;
		}
		if (!print(str, len))
			return -1;
		written += len;
	}

	va_end(parameters);
	return written;
}
