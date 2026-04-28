#include "idt.h"
#include "isr.h"

static struct idt_entry idt[256];
static struct idtr idtr_ptr;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    uint64_t descriptor = (uint64_t)isr;

    idt[vector].isr_low = descriptor & 0xFFFF;
    idt[vector].kernel_cs = 0x08;
    idt[vector].ist = 0;
    idt[vector].attributes = flags;
    idt[vector].isr_mid = (descriptor >> 16) & 0xFFFF;
    idt[vector].isr_high = (descriptor >> 32) & 0xFFFFFFFF;
    idt[vector].reserved = 0;
}

void idt_init() {
    idtr_ptr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr_ptr.base = (uint64_t)&idt[0];

    // Initialize all to 0
    for (int i = 0; i < 256; i++) {
        idt_set_descriptor(i, 0, 0);
    }

    // Set exception handlers
    idt_set_descriptor(0, (void*)isr_divide_by_zero, 0x8E);
    idt_set_descriptor(13, (void*)isr_general_protection_fault, 0x8E);
    idt_set_descriptor(14, (void*)isr_page_fault, 0x8E);
    
    // Set IRQ handlers (PIC is remapped to 32)
    idt_set_descriptor(33, (void*)isr_keyboard, 0x8E);

    __asm__ volatile ("lidt %0" : : "m" (idtr_ptr));
    __asm__ volatile ("sti"); // Enable interrupts
}
