#include <stdint.h>
#include "io.h"
#include "../vga/vga.h"

extern const struct bitmap_font font;

#define IDT_SIZE 256

// IDT entry structure (8 bytes each)
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

// IDT pointer for lidt
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Declare the IDT and its pointer
static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

// IRQ handler function pointers
void (*irq_handlers[16])(void) = { 0 };

// ISR stubs (from isrs.asm)
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();

// Load the IDT (from idt_load.asm)
extern void idt_load(struct idt_ptr*);

// Set a single entry in the IDT
static void idt_set_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void pic_remap() {

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

}


// Install a custom IRQ handler
void irq_install_handler(int irq, void (*handler)(void)) {
    if (irq >= 0 && irq < 16) {
        irq_handlers[irq] = handler;
    } else {
        putstring(200, 60, "Invalid IRQ number", &font, white);
    }
}
// Acknowledge an IRQ to the PIC
void irq_ack(int irq) {
    if (irq >= 8)
        outb(0xA0, 0x20);  // Acknowledge slave
    outb(0x20, 0x20);      // Acknowledge master
}

// Called from isrs.asm with interrupt number
void isr_handler(uint32_t int_no) {
    // Print that the handler was entered

    // Print interrupt number

    if (int_no >= 32 && int_no <= 47) {
        int irq = int_no - 32;


        if (irq_handlers[irq]) {
			irq_handlers[irq]();  // Actually call the handler
        } else {
        }
        irq_ack(irq);

    } else {
        putstring(10, 50, "Interrupt not IRQ (ignored)", &font, white);
    }
}

void test_irq_handler() {
    putstring(80, 100, "intr successful", &font, white);
}


// Initialize the full IDT and IRQ support
void init_interrupts() {
    idtp.limit = sizeof(struct idt_entry) * IDT_SIZE - 1;
    idtp.base  = (uint32_t)&idt;

    // Clear entire IDT
    for (int i = 0; i < IDT_SIZE; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
	pic_remap();

    // Set entries for IRQ0–IRQ15 (interrupts 32–47)
    idt_set_gate(32 + 0,  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_gate(32 + 1,  (uint32_t)isr1,  0x08, 0x8E);
    idt_set_gate(32 + 2,  (uint32_t)isr2,  0x08, 0x8E);
    idt_set_gate(32 + 3,  (uint32_t)isr3,  0x08, 0x8E);
    idt_set_gate(32 + 4,  (uint32_t)isr4,  0x08, 0x8E);
    idt_set_gate(32 + 5,  (uint32_t)isr5,  0x08, 0x8E);
    idt_set_gate(32 + 6,  (uint32_t)isr6,  0x08, 0x8E);
    idt_set_gate(32 + 7,  (uint32_t)isr7,  0x08, 0x8E);
    idt_set_gate(32 + 8,  (uint32_t)isr8,  0x08, 0x8E);
    idt_set_gate(32 + 9,  (uint32_t)isr9,  0x08, 0x8E);
    idt_set_gate(32 + 10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(32 + 11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(32 + 12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(32 + 13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(32 + 14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(32 + 15, (uint32_t)isr15, 0x08, 0x8E);

    idt_load(&idtp);

}
