#include <limine.h>
#include <stddef.h>
#include <stdint.h>
#include <font8x8.h>

// Place base revision into .limine_requests section
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[3] = { 0xf9562b2d5c95a6c8, 0x6a7b384944536bdc, 0 };

// Section markers
__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

// Framebuffer request
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

// Write character to serial port COM1 (0x3F8)
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void print_serial(const char *str) {
    while (*str) {
        outb(0x3f8, *str++);
    }
}

static void halt(void) {
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

static void draw_pixel(struct limine_framebuffer *fb, size_t x, size_t y, uint32_t color) {
    if (x >= fb->width || y >= fb->height) return;
    uint32_t *pix = (uint32_t *)((uint8_t *)fb->address + y * fb->pitch + x * 4);
    *pix = color;
}

static void draw_char(struct limine_framebuffer *fb, size_t x, size_t y, char c, uint32_t fg, uint32_t bg) {
    const uint8_t *glyph = font8x8_basic[(unsigned char)c];
    for (size_t row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (size_t col = 0; col < 8; col++) {
            if ((bits >> col) & 1) {
                draw_pixel(fb, x + col, y + row, fg);
            } else if (bg != 0xFFFFFFFF) {
                draw_pixel(fb, x + col, y + row, bg);
            }
        }
    }
}

static void draw_string(struct limine_framebuffer *fb, size_t x, size_t y, const char *str, uint32_t fg, uint32_t bg) {
    size_t cur_x = x;
    size_t cur_y = y;
    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += 12;
        } else {
            draw_char(fb, cur_x, cur_y, *str, fg, bg);
            cur_x += 8;
        }
        str++;
    }
}

void kmain(void) {
    print_serial("[Kernel] Booted successfully!\n");

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        print_serial("[Kernel] No Framebuffer available!\n");
        halt();
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    // Solid Black Background (0x000000)
    uint32_t bg_color = 0x000000;
    uint32_t white = 0xFFFFFF;

    for (size_t y = 0; y < fb->height; y++) {
        for (size_t x = 0; x < fb->width; x++) {
            draw_pixel(fb, x, y, bg_color);
        }
    }

    // Pure white text on black background
    draw_string(fb, 100, 100, "Welcome to Imperfect OS!", white, bg_color);

    print_serial("[Kernel] Framebuffer rendered successfully!\n");

    halt();
}
