#include "nun/boot.h"
#include "nun/framebuffer.h"
#include "nun/pmm.h"
#include "nun/serial.h"
#include "nun/services.h"

#include <stddef.h>
#include <stdint.h>

static __attribute__((noreturn)) void nun_halt(const char *reason)
{
    nun_serial_write("[NUN::HALT] ");
    nun_serial_write(reason);
    nun_serial_write("\n");
    for (;;) __asm__ volatile ("cli; hlt");
}

__attribute__((noreturn))
void kernel_main(void)
{
    const struct limine_memmap_response *memory_map;
    const struct limine_hhdm_response *hhdm;
    const struct limine_framebuffer_response *framebuffer;
    const struct limine_bootloader_info_response *boot_info;
    void *first_page;

    (void)nun_serial_init();
    nun_serial_write("\n[NUN::KERNEL] entry x86_64\n");

    if (nun_limine_base_revision[2] != 0u)
        nun_halt("unsupported Limine base revision");

    if (nun_limine_memmap_request.response == 0 ||
        nun_limine_hhdm_request.response == 0)
        nun_halt("required boot response missing");

    memory_map = nun_limine_memmap_request.response;
    hhdm = nun_limine_hhdm_request.response;

    if (memory_map->entries == 0 || memory_map->entry_count == 0u)
        nun_halt("invalid memory map");

    nun_serial_write("[NUN::BOOT] memory map entries=");
    nun_serial_write_u64(memory_map->entry_count);
    nun_serial_write("\n[NUN::BOOT] HHDM offset=");
    nun_serial_write_hex(hhdm->offset);
    nun_serial_write("\n");

    boot_info = nun_limine_bootloader_info_request.response;
    if (boot_info != 0 && boot_info->name != 0) {
        nun_serial_write("[NUN::BOOT] loader=");
        nun_serial_write(boot_info->name);
        if (boot_info->version != 0) {
            nun_serial_write(" ");
            nun_serial_write(boot_info->version);
        }
        nun_serial_write("\n");
    }

    framebuffer = nun_limine_framebuffer_request.response;
    if (nun_framebuffer_init(framebuffer)) {
        nun_framebuffer_boot_marker();
        nun_serial_write("[NUN::DISPLAY] black-yellow framebuffer marker ready\n");
    } else {
        nun_serial_write("[NUN::DISPLAY] framebuffer unavailable; serial remains active\n");
    }

    if (!nun_pmm_init(memory_map, hhdm->offset))
        nun_halt("physical memory map rejected");

    nun_serial_write("[NUN::MEMORY] usable bytes=");
    nun_serial_write_u64(nun_pmm_usable_bytes());
    nun_serial_write("\n");

    first_page = nun_pmm_alloc_pages(1u);
    if (first_page == 0)
        nun_halt("initial physical page allocation failed");

    nun_serial_write("[NUN::MEMORY] first page=");
    nun_serial_write_hex((unsigned long long)(uintptr_t)first_page);
    nun_serial_write("\n");

    if (!nun_services_smoke_init())
        nun_halt("linked system-service contract check failed");

    nun_serial_write("[NUN::SERVICES] Kloz CO SaintSI ReaperCripple contracts passed\n");
    nun_serial_write("[NUN::READY] early boot foundation initialized\n");

    for (;;) __asm__ volatile ("cli; hlt");
}
