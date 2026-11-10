#include "nun/boot.h"

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[4] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
volatile uint64_t nun_limine_base_revision[3] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
volatile struct limine_memmap_request nun_limine_memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID, .revision = 0, .response = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_hhdm_request nun_limine_hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID, .revision = 0, .response = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request nun_limine_framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 0, .response = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_bootloader_info_request nun_limine_bootloader_info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST_ID, .revision = 0, .response = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[2] = LIMINE_REQUESTS_END_MARKER;
