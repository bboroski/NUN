# NUN x86_64 QEMU kernel bring-up

This executable target is separate from the existing host-side CMake integration build.

## Implemented in this milestone

- GRUB Multiboot2 entry and transition from 32-bit protected mode to 64-bit long mode.
- Temporary identity map for the first GiB using 2 MiB pages.
- COM1 serial diagnostics.
- Multiboot2 memory-map parsing and a bounded physical-page bitmap allocator for the first GiB.
- IDT exception stubs, legacy PIC remapping and a 100 Hz PIT IRQ0.
- CI builds an ISO, boots it in QEMU and checks serial output including a timer interrupt.

## Build and run

On Debian or Ubuntu install `gcc`, `binutils`, `grub-pc-bin`, `grub-common`, `xorriso` and `qemu-system-x86`.

```sh
bash scripts/build-qemu.sh
bash scripts/run-qemu.sh
```

The serial console is COM1 (stdio). Expected diagnostics include kernel entry, the Multiboot2 pointer, usable tracked pages, one allocated page, IDT/PIC/PIT initialization and periodic timer messages.

## Explicit limitations

This is a bring-up milestone, not a production kernel. The page allocator tracks only physical addresses below 1 GiB; the temporary page tables identity-map only that range. The allocator is a single-core bitmap without locking. The interrupt layer currently enables only PIT IRQ0; other vectors route to a fatal diagnostic path. There is no userspace, scheduler, SMP, ACPI, PCI/USB, VFS, network stack, Secure Boot or graphical compositor yet. Those must be added as separately tested milestones.

The existing top-level CMake build remains a host integration/test build and is not used to claim a bootable image.
