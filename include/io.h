#ifndef IO_H
#define IO_H

#include <stdint.h>


static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile(
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port));

    return value;
}

#endif