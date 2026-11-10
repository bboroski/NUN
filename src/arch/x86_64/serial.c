#include "nun/serial.h"
#include <stdint.h>

#define COM1 0x3F8u

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

bool nun_serial_init(void)
{
    outb(COM1 + 1u, 0x00u);
    outb(COM1 + 3u, 0x80u);
    outb(COM1 + 0u, 0x01u);
    outb(COM1 + 1u, 0x00u);
    outb(COM1 + 3u, 0x03u);
    outb(COM1 + 2u, 0xC7u);
    outb(COM1 + 4u, 0x0Bu);
    return (inb(COM1 + 5u) != 0xFFu);
}

void nun_serial_putc(char value)
{
    unsigned int spin;
    for (spin = 0u; spin < 1000000u; ++spin) {
        if ((inb(COM1 + 5u) & 0x20u) != 0u) {
            outb(COM1, (uint8_t)value);
            return;
        }
    }
}

void nun_serial_write(const char *text)
{
    if (text == 0) return;
    while (*text != '\0') {
        if (*text == '\n') nun_serial_putc('\r');
        nun_serial_putc(*text++);
    }
}

void nun_serial_write_hex(unsigned long long value)
{
    static const char digits[] = "0123456789ABCDEF";
    int shift;
    nun_serial_write("0x");
    for (shift = 60; shift >= 0; shift -= 4)
        nun_serial_putc(digits[(value >> (unsigned int)shift) & 0x0Fu]);
}

void nun_serial_write_u64(unsigned long long value)
{
    char buffer[21];
    unsigned int used = 0u;
    if (value == 0u) { nun_serial_putc('0'); return; }
    while (value != 0u && used < sizeof(buffer)) {
        buffer[used++] = (char)('0' + (value % 10u));
        value /= 10u;
    }
    while (used != 0u) nun_serial_putc(buffer[--used]);
}
