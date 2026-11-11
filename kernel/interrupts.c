#include "nun/interrupts.h"
#include "nun/serial.h"

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idtr_t;

static idt_entry_t g_idt[256];
static volatile uint64_t g_timer_ticks;

extern void isr_divide_error(void);
extern void isr_invalid_opcode(void);
extern void isr_double_fault(void);
extern void isr_general_protection(void);
extern void isr_page_fault(void);
extern void isr_timer(void);

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void idt_set_gate(uint8_t vector, void (*handler)(void))
{
    const uint64_t address = (uint64_t)(uintptr_t)handler;
    g_idt[vector].offset_low = (uint16_t)(address & 0xFFFFu);
    g_idt[vector].selector = 0x08u;
    g_idt[vector].ist = 0u;
    g_idt[vector].attributes = 0x8Eu;
    g_idt[vector].offset_middle = (uint16_t)((address >> 16) & 0xFFFFu);
    g_idt[vector].offset_high = (uint32_t)(address >> 32);
    g_idt[vector].reserved = 0u;
}

static void pic_remap_and_mask(void)
{
    outb(0x20u, 0x11u);
    outb(0xA0u, 0x11u);
    outb(0x21u, 0x20u);
    outb(0xA1u, 0x28u);
    outb(0x21u, 0x04u);
    outb(0xA1u, 0x02u);
    outb(0x21u, 0x01u);
    outb(0xA1u, 0x01u);
    outb(0x21u, 0xFEu); /* Unmask only IRQ0 (PIT). */
    outb(0xA1u, 0xFFu);
}

static void pit_init_1khz(void)
{
    const uint16_t divisor = 1193u; /* 1,193,182 Hz / 1,193 ~= 1,000.15 Hz. */
    outb(0x43u, 0x36u);
    outb(0x40u, (uint8_t)(divisor & 0xFFu));
    outb(0x40u, (uint8_t)(divisor >> 8));
}

void nun_interrupts_init(void)
{
    idtr_t descriptor;
    __asm__ volatile ("cli");
    for (uint32_t index = 0u; index < 256u; ++index) {
        g_idt[index] = (idt_entry_t){0};
    }
    idt_set_gate(0u, isr_divide_error);
    idt_set_gate(6u, isr_invalid_opcode);
    idt_set_gate(8u, isr_double_fault);
    idt_set_gate(13u, isr_general_protection);
    idt_set_gate(14u, isr_page_fault);
    idt_set_gate(32u, isr_timer);
    descriptor.limit = (uint16_t)(sizeof(g_idt) - 1u);
    descriptor.base = (uint64_t)(uintptr_t)g_idt;
    __asm__ volatile ("lidt %0" : : "m"(descriptor));
    pic_remap_and_mask();
    pit_init_1khz();
}

void nun_interrupts_enable(void)
{
    __asm__ volatile ("sti" ::: "memory");
}

void nun_timer_interrupt(void)
{
    ++g_timer_ticks;
    if (g_timer_ticks == 1000u) {
        nun_serial_write("[IRQ] PIT timer interrupt observed (1000 ticks)\n");
    }
    outb(0x20u, 0x20u);
}

void nun_exception_handler(unsigned long vector)
{
    nun_serial_write("[PANIC] CPU exception vector ");
    nun_serial_write_u64((uint64_t)vector);
    nun_serial_write("\n");
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
