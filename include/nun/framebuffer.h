#ifndef NUN_FRAMEBUFFER_H
#define NUN_FRAMEBUFFER_H
#include <stdbool.h>
#include <stdint.h>
#include "limine.h"
bool nun_framebuffer_init(const struct limine_framebuffer_response *response);
void nun_framebuffer_boot_marker(void);
#endif
