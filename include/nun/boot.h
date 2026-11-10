#ifndef NUN_BOOT_H
#define NUN_BOOT_H
#include <stdint.h>
#include "limine.h"
extern volatile uint64_t nun_limine_base_revision[3];
extern volatile struct limine_memmap_request nun_limine_memmap_request;
extern volatile struct limine_hhdm_request nun_limine_hhdm_request;
extern volatile struct limine_framebuffer_request nun_limine_framebuffer_request;
extern volatile struct limine_bootloader_info_request nun_limine_bootloader_info_request;
#endif
