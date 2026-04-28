#ifndef VIDEO_H
#define VIDEO_H

#include "limine.h"
#include <stdint.h>

void video_init(struct limine_framebuffer* fb);
void put_pixel(int x, int y, uint32_t color);
void draw_char(char c, int x, int y, uint32_t color);
void draw_string(const char* str, int x, int y, uint32_t color);
void clear_screen(uint32_t color);

#endif
