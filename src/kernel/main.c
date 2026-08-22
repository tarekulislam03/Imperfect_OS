#include <limine.h>
#include <stddef.h>
#include "keyboard.h"
#include "serial.h"
#include "framebuffer.h"
#include "terminal.h"
#include "delay.h"

// halt funtion
static void halt(void) {
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void kmain(void) {

    //check src/drivers/serial.c for more detailed code review for this init_serial();
    init_serial();
    print_serial("[Kernel] Booting...\n");

    //check src/graphics/framebuffer.c for more detailed review for init_framebuffer();
    init_framebuffer();
    if (global_fb == NULL) {
        halt();
    }

    // booting animation
    clear_screen(BG_COLOR);
    draw_string(global_fb, 100, 100, "Booting into Imperfect_OS", TEXT_COLOR, TRANSPARENT);

    draw_char(global_fb, 100 + 25 * 16, 100, '.', TEXT_COLOR, TRANSPARENT);
    delay_ms(120);

    draw_char(global_fb, 100 + 26 * 16, 100, '.', TEXT_COLOR, TRANSPARENT);
    delay_ms(120);

    draw_char(global_fb, 100 + 27 * 16, 100, '.', TEXT_COLOR, TRANSPARENT);
    delay_ms(260);

    clear_screen(BG_COLOR);
    draw_string(global_fb, 100, 100, "Welcome to Imperfect_OS!", TEXT_COLOR, TRANSPARENT);

    // typing loop. the keyboard_getchar() funtion is from src/drivers/keyboard.c; check for detailed code review.
    while (1) {
        char c = keyboard_getchar();
        if (c != 0) {
            putchar(c);
        }
    }

    halt();
}
