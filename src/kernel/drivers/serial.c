#include "serial.h"

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void init_serial(void) {
    outb(0x3f8 + 1, 0x00);    // Disable interrupts
    outb(0x3f8 + 3, 0x80);    // Enable DLAB
    outb(0x3f8 + 0, 0x03);    // Set divisor to 3 (38400 baud)
    outb(0x3f8 + 1, 0x00);
    outb(0x3f8 + 3, 0x03);    // 8 bits, no parity, 1 stop bit
    outb(0x3f8 + 2, 0xC7);    // Enable FIFO
    outb(0x3f8 + 4, 0x0B);
}

void print_serial(const char *str) {
    while (*str) {
        outb(0x3f8, *str++);
    }
}
