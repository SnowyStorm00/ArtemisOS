#include "isr.h"
#include "kprintf.h"
#include "pic.h"
#include "io.h"
#include "video.h"

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

__attribute__((interrupt)) void isr_keyboard(struct interrupt_frame* frame) {
    uint8_t scancode = inb(0x60);
    
    // Only process key down (scancode < 0x80)
    if (scancode < 0x80) {
        kprintf("IRQ1: Keyboard scancode 0x%x\n", scancode);
        draw_string("KEY", cursor_x, 100, 0x00FFFF);
        cursor_x += 32;
        if (cursor_x > 800) cursor_x = 20; // Basic wrap around
    }

    pic_send_eoi(1);
}
