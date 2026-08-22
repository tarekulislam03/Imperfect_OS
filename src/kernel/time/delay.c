#include "delay.h"

void delay_ms(uint64_t ms) {
    for (volatile uint64_t i = 0; i < ms * 40000; i++) {
        __asm__ volatile ("pause");
    }
}
