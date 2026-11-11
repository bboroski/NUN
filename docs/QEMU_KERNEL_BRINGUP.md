# NUN x86_64 QEMU kernel bring-up

This freestanding kernel target is separate from the host-side CMake integration build.

## Phase A capabilities

- GRUB Multiboot2 entry and transition from 32-bit protected mode to x86_64 long mode.
- Temporary identity mapping of physical addresses below 1 GiB using 2 MiB pages.
- COM1 serial diagnostics and a polling command loop: ping returns NUN: pong; help lists commands.
- Multiboot2 memory-map parsing with bounds checks and a bounded physical-page bitmap allocator for the identity-mapped first GiB.
- One-page write/read/clear self-test before the kernel reports readiness.
- IDT exception stubs, legacy PIC remapping and PIT IRQ0 nominally configured at 1 kHz.
- CI builds an ISO, launches QEMU, checks memory and interrupt diagnostics, and sends ping over the serial console.

## Build and run

On Debian or Ubuntu install gcc, binutils, grub-pc-bin, grub-common, mtools, xorriso and qemu-system-x86.

    bash scripts/build-qemu.sh
    bash scripts/run-qemu.sh

The serial console is COM1 (stdio). Once NUN Kernel Ready appears, type ping and expect NUN: pong.

## Explicit limitations

This is a bring-up milestone, not a production kernel. The page allocator tracks only physical addresses below 1 GiB; the temporary page tables identity-map only that range. The allocator is a single-core bitmap without a free path or synchronization. The interrupt layer currently enables only PIT IRQ0 through the legacy PIC; APIC/IOAPIC, keyboard, UART interrupts, SMP, ACPI, PCI/USB, VFS, networking, Secure Boot, user mode, scheduler, system services and graphics are not implemented by this milestone. The 1 kHz timer is the PIT's nominal programmed rate, not a claim of nanosecond precision.

The top-level CMake build remains a host integration/test build and is not a bootable OS image. A successful CI job is required before this milestone is described as QEMU-verified.
