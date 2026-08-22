#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <limine.h>
#include <stddef.h>
#include <stdint.h>

// Colors
#define BG_COLOR    0x000000 // Black
#define TEXT_COLOR  0xFFFFFF // White
#define CYAN_COLOR  0x38BDF8 // Sky Cyan
#define TRANSPARENT 0xFFFFFFFF

extern struct limine_framebuffer *global_fb;
extern struct limine_framebuffer_request framebuffer_request;

void init_framebuffer(void);
void draw_pixel(struct limine_framebuffer *fb, size_t x, size_t y, uint32_t color);
void draw_char(struct limine_framebuffer *fb, size_t x, size_t y, char c, uint32_t fg, uint32_t bg);
void draw_string(struct limine_framebuffer *fb, size_t x, size_t y, const char *str, uint32_t fg, uint32_t bg);
void clear_screen(uint32_t bg_color);

#endif