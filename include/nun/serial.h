#ifndef NUN_SERIAL_H
#define NUN_SERIAL_H

#include <stdint.h>

void nun_serial_init(void);
void nun_serial_write_char(char value);
void nun_serial_write(const char *text);
void nun_serial_write_u64(uint64_t value);
void nun_serial_write_hex(uint64_t value);

#endif
