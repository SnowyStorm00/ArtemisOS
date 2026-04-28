#include "isr.h"
#include "kprintf.h"

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
