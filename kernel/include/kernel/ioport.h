#ifndef INCLUDE_IOPORT_H_
#define INCLUDE_IOPORT_H_

#include <stdint.h>

// write byte
void outb(uint16_t port, uint8_t byte);

// read byte
uint8_t inb(uint16_t port);

void io_wait(void);

#endif // INCLUDE_IOPORT_H_