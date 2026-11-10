#ifndef NUN_SERIAL_H
#define NUN_SERIAL_H
#include <stdbool.h>
bool nun_serial_init(void);
void nun_serial_putc(char value);
void nun_serial_write(const char *text);
void nun_serial_write_u64(unsigned long long value);
void nun_serial_write_hex(unsigned long long value);
#endif
