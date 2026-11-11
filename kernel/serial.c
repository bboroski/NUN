#include "nun/serial.h"
#include <stddef.h>

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

void nun_serial_init(void)
{
    outb((uint16_t)(COM1 + 1u), 0x00u);
    outb((uint16_t)(COM1 + 3u), 0x80u);
    outb((uint16_t)COM1, 0x01u);
    outb((uint16_t)(COM1 + 1u), 0x00u);
    outb((uint16_t)(COM1 + 3u), 0x03u);
    outb((uint16_t)(COM1 + 2u), 0xC7u);
    outb((uint16_t)(COM1 + 4u), 0x03u);
}

void nun_serial_write_char(char value)
{
    while ((inb((uint16_t)(COM1 + 5u)) & 0x20u) == 0u) {
        __asm__ volatile ("pause");
    }
    outb((uint16_t)COM1, (uint8_t)value);
}

void nun_serial_write(const char *text)
{
    if (text == NULL) {
        return;
    }
    while (*text != '\0') {
        if (*text == '\n') {
            nun_serial_write_char('\r');
        }
        nun_serial_write_char(*text);
        ++text;
    }
}

void nun_serial_write_u64(uint64_t value)
{
    char digits[20];
    unsigned int used = 0u;
    if (value == 0u) {
        nun_serial_write_char('0');
        return;
    }
    while (value != 0u && used < (unsigned int)sizeof(digits)) {
        digits[used] = (char)('0' + (char)(value % 10u));
        value /= 10u;
        ++used;
    }
    while (used > 0u) {
        --used;
        nun_serial_write_char(digits[used]);
    }
}

void nun_serial_write_hex(uint64_t value)
{
    static const char digits[] = "0123456789ABCDEF";
    int shift;
    nun_serial_write("0x");
    for (shift = 60; shift >= 0; shift -= 4) {
        nun_serial_write_char(digits[(value >> (unsigned int)shift) & 0x0Fu]);
    }
}
