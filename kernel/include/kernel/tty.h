#ifndef KERNEL_TTY_H_
#define KERNEL_TTY_H_

#include <stddef.h>
#include <kernel/ioport.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_newline(void);
void terminal_cursor_move(void);

#endif
