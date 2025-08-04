#ifndef INTR_H
#define INTR_H

#include <stdint.h>

// Structure used for an interrupt descriptor table entry
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

// Structure used to load the IDT pointer
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Initialize IDT, PIC, and enable IRQs
void init_interrupts(void);

// Install a handler for the given IRQ number (0-15)
void irq_install_handler(int irq, void (*handler)(void));

// Acknowledge the PIC that IRQ has been handled
void irq_ack(int irq);
void test_irq_handler(void);

#endif // INTR_H

