#include "isr.h"
#include "kprintf.h"
#include "pic.h"
#include "io.h"
#include "video.h"
#include "serial.h"
#include "keyboard.h"

__attribute__((interrupt)) void isr_divide_by_zero(struct interrupt_frame* frame) {
    kprintf("\n*** EXCEPTION: DIVIDE BY ZERO ***\n");
    kprintf("RIP: 0x%x  CS: 0x%x\n", frame->rip, frame->cs);
    for(;;) __asm__("hlt");
}

__attribute__((interrupt)) void isr_general_protection_fault(struct interrupt_frame* frame, uint64_t error_code) {
    kprintf("\n*** EXCEPTION: GENERAL PROTECTION FAULT ***\n");
    kprintf("Error Code: 0x%x\n", error_code);
    kprintf("RIP: 0x%x  CS: 0x%x\n", frame->rip, frame->cs);
    for(;;) __asm__("hlt");
}

__attribute__((interrupt)) void isr_page_fault(struct interrupt_frame* frame, uint64_t error_code) {
    kprintf("\n*** EXCEPTION: PAGE FAULT ***\n");
    kprintf("Error Code: 0x%x\n", error_code);
    kprintf("RIP: 0x%x  CS: 0x%x\n", frame->rip, frame->cs);
    for(;;) __asm__("hlt");
}

static int cursor_x = 20;
static int cursor_y = 100;

__attribute__((interrupt)) void isr_keyboard(struct interrupt_frame* frame) {
    // Read all available bytes from the keyboard buffer so it doesn't freeze!
    while (inb(0x64) & 1) {
        uint8_t scancode = inb(0x60);
        
        // Only process key down (scancode < 0x80)
        if (scancode < 0x80) {
            char c = keyboard_scancode_to_ascii(scancode);
            if (c != 0) {
                // Handle Enter key
                if (c == '\n') {
                    cursor_x = 20;
                    cursor_y += 16;
                }
                // Handle Backspace
                else if (c == '\b') {
                    if (cursor_x > 20) {
                        cursor_x -= 8;
                        // To actually erase, we must draw a solid 8x8 black block
                        for (int r = 0; r < 8; r++) {
                            for (int col = 0; col < 8; col++) {
                                put_pixel(cursor_x + col, cursor_y + r, 0x000000);
                            }
                        }
                    }
                }
                // Handle normal characters
                else {
                    // Debug print to serial to see what the OS thinks you typed
                    serial_write_string("Typed: ");
                    serial_write_char(c);
                    serial_write_string("\n");

                    draw_char(c, cursor_x, cursor_y, 0x00FFFF);
                    cursor_x += 8;
                }

                // Wrap around horizontally
                if (cursor_x >= 780) {
                    cursor_x = 20;
                    cursor_y += 16;
                }

                // Wrap around vertically
                if (cursor_y >= 580) {
                    cursor_y = 100;
                    // Ideally we would clear the screen or scroll here!
                }
            }
        }
    }

    pic_send_eoi(1);
}
