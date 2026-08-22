#include "framebuffer.h"
#include <font8x8.h>

struct limine_framebuffer *global_fb = NULL;

void init_framebuffer(void) {
    if (framebuffer_request.response != NULL && framebuffer_request.response->framebuffer_count > 0) {
        global_fb = framebuffer_request.response->framebuffers[0];
    }
}

void draw_pixel(struct limine_framebuffer *fb, size_t x, size_t y, uint32_t color) {
    if (fb == NULL || x >= fb->width || y >= fb->height) return;
    uint32_t *pix = (uint32_t *)((uint8_t *)fb->address + y * fb->pitch + x * 4);
    *pix = color;
}

// 16x16 Character Renderer (Scaled 2x from 8x8 font)
void draw_char(struct limine_framebuffer *fb, size_t x, size_t y, char c, uint32_t fg, uint32_t bg) {
    if (fb == NULL) return;
    unsigned char idx = (unsigned char)c;
    if (idx >= 128) idx = '?';

    const uint8_t *glyph = font8x8_basic[idx];
    for (size_t row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (size_t col = 0; col < 8; col++) {
            uint32_t color = ((bits >> col) & 1) ? fg : bg;
            if (color != 0xFFFFFFFF) {
                // Render 2x2 pixels for 16x16 scaling
                draw_pixel(fb, x + col * 2,     y + row * 2,     color);
                draw_pixel(fb, x + col * 2 + 1, y + row * 2,     color);
                draw_pixel(fb, x + col * 2,     y + row * 2 + 1, color);
                draw_pixel(fb, x + col * 2 + 1, y + row * 2 + 1, color);
            }
        }
    }
}

void draw_string(struct limine_framebuffer *fb, size_t x, size_t y, const char *str, uint32_t fg, uint32_t bg) {
    if (fb == NULL) return;
    size_t cur_x = x;
    size_t cur_y = y;
    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += 24;
        } else {
            draw_char(fb, cur_x, cur_y, *str, fg, bg);
            cur_x += 16;
        }
        str++;
    }
}

void clear_screen(uint32_t bg_color) {
    if (global_fb == NULL) return;
    for (size_t y = 0; y < global_fb->height; y++) {
        for (size_t x = 0; x < global_fb->width; x++) {
            draw_pixel(global_fb, x, y, bg_color);
        }
    }
}