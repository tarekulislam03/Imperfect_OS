#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void outb(uint16_t port, uint8_t value);
void init_serial(void);
void print_serial(const char *str);

#endif
