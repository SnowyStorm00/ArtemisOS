#include "limine.h"
#include "font.h"
#include "io.h"
#include "serial.h"
#include "kprintf.h"
#include "gdt.h"
#include "idt.h"
#include <stdint.h>
#include <stddef.h>

__attribute__((used, section(".limine_reqs")))
static volatile struct limine_framebuffer_request fb_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Clean, standard coordinate system (0,0 is Top-Left)
void put_pixel(struct limine_framebuffer* fb, int x, int y, uint32_t color) {
    if (x < 0 || x >= (int)fb->width || y < 0 || y >= (int)fb->height) return;
    volatile uint8_t* fb_ptr = (volatile uint8_t*)fb->address;
    *(volatile uint32_t*)(fb_ptr + y * fb->pitch + x * 4) = color;
}

void draw_char(struct limine_framebuffer* fb, char c, int x, int y, uint32_t color) {
    uint8_t* glyph = font8x8_basic[(unsigned char)c];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            // Standard MSB-first bit shifting
            if ((glyph[row] >> (7 - col)) & 1) {
                put_pixel(fb, x + col, y + row, color);
            }
        }
    }
}

void draw_string(struct limine_framebuffer* fb, const char* str, int x, int y, uint32_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        draw_char(fb, str[i], x + (i * 8), y, color);
    }
}

extern "C" void _start(void) {
    serial_init();
    kprintf("ArtemisOS Booting...\n");
    kprintf("Serial initialized at COM1 (0x%x)\n", COM1);

    gdt_init();
    kprintf("GDT Initialized.\n");

    idt_init();
    kprintf("IDT Initialized.\n");

    if (fb_req.response == NULL || fb_req.response->framebuffer_count < 1) {
        kprintf("Error: Framebuffer not found!\n");
        for (;;) { __asm__("hlt"); }
    }

    struct limine_framebuffer *fb = fb_req.response->framebuffers[0];

    // Clear screen to Black
    for (size_t y = 0; y < fb->height; y++) {
        for (size_t x = 0; x < fb->width; x++) {
            put_pixel(fb, x, y, 0x000000);
        }
    }

    draw_string(fb, "ARTEMIS OS V0.0.1", 20, 20, 0xFFFFFF);
    draw_string(fb, "SYSTEM RENDER TEST", 20, 40, 0x00FF00);

    // Grid check
    int startX = 20;
    int startY = 60;
    int spacing = 16; 

    for (int i = 32; i < 128; i++) {
        int col = (i - 32) % 16;
        int row = (i - 32) / 16;
        draw_char(fb, (char)i, startX + (col * spacing), startY + (row * spacing), 0x00FF00);
    }

    // Keyboard logic
    for (;;) {
        if (inb(0x64) & 1) { // Check if data is available
            uint8_t scancode = inb(0x60);
            if (scancode == 0x1E) { // 'A' key
                draw_string(fb, "A DETECTED", 20, 200, 0xFF00FF);
            }
        }
    }
}