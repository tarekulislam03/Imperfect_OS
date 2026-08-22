#include <stdint.h>
#include "keyboard.h"
#include "io.h"

static int shift_pressed = 0;

static const char keymap[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const char keymap_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

char keyboard_getchar(void)
{   
    // checks if output buffer is empty or not
    while (!(inb(0x64) & 1))
        ;

    // getting scancode from port 0x60. 0x60 primarily refers to the I/O port address used for communication with the AT keyboard controller (Intel 8042) in IBM PC-compatible systems.  
    uint8_t scancode = inb(0x60);

    // checking if shift is pressed. 0x2A = left shift Make code & 0x36 is right shift Make code. 
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }

    // check if shift key release or not. 0xAA= left shift Break code & 0xB6= right shift Break code
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return 0;
    }

    // ignore any other key-release code
    if (scancode & 0x80)
        return 0;

    if (scancode >= 128)
        return 0;

    return shift_pressed ? keymap_shift[scancode] : keymap[scancode];
}