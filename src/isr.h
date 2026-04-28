#ifndef ISR_H
#define ISR_H

#include <stdint.h>

struct interrupt_frame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

__attribute__((interrupt)) void isr_divide_by_zero(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr_general_protection_fault(struct interrupt_frame* frame, uint64_t error_code);
__attribute__((interrupt)) void isr_page_fault(struct interrupt_frame* frame, uint64_t error_code);
__attribute__((interrupt)) void isr_keyboard(struct interrupt_frame* frame);

#endif
