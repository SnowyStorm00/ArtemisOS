#include "limine.h"
#include "io.h"
#include "serial.h"
#include "kprintf.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "video.h"
#include "pmm.h"
#include <stdint.h>
#include <stddef.h>

__attribute__((used, section(".limine_reqs")))
static volatile struct limine_framebuffer_request fb_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_reqs")))
static volatile struct limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

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

    if (memmap_req.response == NULL) {
        kprintf("Error: Memory map not found!\n");
        for (;;) { __asm__("hlt"); }
    }

    struct limine_framebuffer *fb = fb_req.response->framebuffers[0];
    video_init(fb);

    // Clear screen to Black
    clear_screen(0x000000);

    draw_string("ARTEMIS OS V0.0.2", 20, 20, 0xFFFFFF);
    draw_string("SYSTEM RENDER TEST", 20, 40, 0x00FF00);

    // Grid check
    int startX = 20;
    int startY = 60;
    int spacing = 16; 

    for (int i = 32; i < 128; i++) {
        int col = (i - 32) % 16;
        int row = (i - 32) / 16;
        draw_char((char)i, startX + (col * spacing), startY + (row * spacing), 0x00FF00);
    }

    // Remap the PIC so IRQs don't collide with CPU exceptions
    pic_remap(0x20, 0x28);
    kprintf("PIC Remapped to 0x20 and 0x28.\n");

    // Initialize Physical Memory Manager
    pmm_init(memmap_req.response);

    kprintf("Testing PMM Allocation...\n");
    void* p1 = pmm_alloc();
    void* p2 = pmm_alloc();
    void* p3 = pmm_alloc();
    
    kprintf("Allocated Page 1: 0x%x\n", (uint64_t)p1);
    kprintf("Allocated Page 2: 0x%x\n", (uint64_t)p2);
    kprintf("Allocated Page 3: 0x%x\n", (uint64_t)p3);

    pmm_free(p2);
    kprintf("Freed Page 2. Reallocating...\n");
    
    void* p4 = pmm_alloc();
    kprintf("Allocated Page 4: 0x%x\n", (uint64_t)p4);

    kprintf("System initialization complete. Going to sleep.\n");

    // Infinite sleep loop - CPU will only wake up when an interrupt fires!
    for (;;) {
        __asm__ volatile ("hlt");
    }
}