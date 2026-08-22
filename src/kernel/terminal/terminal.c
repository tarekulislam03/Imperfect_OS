#include "terminal.h"
#include "framebuffer.h"
#include "serial.h"
#include <stddef.h>

static size_t cursor_x = 100;
static size_t cursor_y = 140;
static const size_t margin_x = 100;

void putchar(char c) {
    outb(0x3f8, c);

    if (global_fb == NULL) return;

    if (c == '\n') {
        cursor_x = margin_x;
        cursor_y += 24;
    } else if (c == '\b') {
        if (cursor_x >= margin_x + 16) {
            cursor_x -= 16;
            draw_char(global_fb, cursor_x, cursor_y, ' ', BG_COLOR, BG_COLOR);
        }
    } else {
        draw_char(global_fb, cursor_x, cursor_y, c, TEXT_COLOR, BG_COLOR);
        cursor_x += 16;
        if (cursor_x + 16 >= global_fb->width) {
            cursor_x = margin_x;
            cursor_y += 24;
        }
    }
}
