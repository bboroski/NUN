#include "nun/interrupts.h"
#include "nun/memory.h"
#include "nun/serial.h"

#include <stdint.h>

#define MULTIBOOT2_BOOT_MAGIC 0x36D76289u

void kmain(uintptr_t multiboot2_info, uint32_t boot_magic)
{
    void *first_frame;

    nun_serial_init();
    nun_serial_write("NUN x86_64 kernel entry\n");

    if (boot_magic != MULTIBOOT2_BOOT_MAGIC) {
        nun_serial_write("[BOOT] invalid Multiboot2 handoff magic\n");
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }
    nun_serial_write("[BOOT] Multiboot2 handoff accepted\n");

    if (nun_memory_init(multiboot2_info) == 0) {
        nun_serial_write("[MM] invalid or missing Multiboot2 memory map\n");
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }
    nun_serial_write("[MM] usable memory below 1 GiB: ");
    nun_serial_write_u64(nun_memory_usable_bytes() / (1024u * 1024u));
    nun_serial_write(" MiB\n");

    first_frame = nun_memory_allocate_frame();
    if (first_frame == NULL) {
        nun_serial_write("[MM] early physical frame allocation failed\n");
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }
    nun_serial_write("[MM] allocated and zeroed frame at ");
    nun_serial_write_hex((uint64_t)(uintptr_t)first_frame);
    nun_serial_write("\n");

    nun_interrupts_init();
    nun_serial_write("[IRQ] IDT installed; PIC remapped; PIT programmed near 1 kHz\n");
    nun_serial_write("NUN Kernel Ready\n");
    nun_interrupts_enable();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
