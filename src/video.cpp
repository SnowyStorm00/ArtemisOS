#include "video.h"
#include "font.h"

static struct limine_framebuffer* global_fb = 0;

void video_init(struct limine_framebuffer* fb) {
    global_fb = fb;
}

void put_pixel(int x, int y, uint32_t color) {
    if (!global_fb) return;
    if (x < 0 || x >= (int)global_fb->width || y < 0 || y >= (int)global_fb->height) return;
    volatile uint8_t* fb_ptr = (volatile uint8_t*)global_fb->address;
    *(volatile uint32_t*)(fb_ptr + y * global_fb->pitch + x * 4) = color;
}

void draw_char(char c, int x, int y, uint32_t color) {
    uint8_t* glyph = font8x8_basic[(unsigned char)c];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((glyph[row] >> (7 - col)) & 1) {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

void draw_string(const char* str, int x, int y, uint32_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        draw_char(str[i], x + (i * 8), y, color);
    }
}

void clear_screen(uint32_t color) {
    if (!global_fb) return;
    for (uint64_t y = 0; y < global_fb->height; y++) {
        for (uint64_t x = 0; x < global_fb->width; x++) {
            put_pixel(x, y, color);
        }
    }
}
