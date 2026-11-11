# NUN x86_64 QEMU bootstrap — phase A

## Scope and maturity

This phase adds an executable Multiboot2 x86_64 bring-up path. It is intentionally a small kernel substrate, not a claim that the complete NUN operating system is implemented. The only currently exercised machine profile is QEMU `q35` with one x86_64 CPU and 512 MiB RAM.

The boot path is:

1. GRUB loads the Multiboot2 ELF image and passes the boot-information address and magic.
2. The 32-bit protected-mode entry builds identity mappings for the first GiB with 2 MiB pages, enables PAE, EFER.LME and paging, then enters 64-bit mode.
3. The kernel initializes COM1 and validates the Multiboot2 memory-map tag.
4. The early physical-frame allocator returns and zeroes one 4 KiB frame from usable RAM above the kernel, boot-information block and boot modules.
5. The kernel installs exception gates for vectors 0, 6, 8, 13 and 14; remaps the legacy PIC; programs PIT channel 0 near 1 kHz; then enables interrupts.
6. The serial smoke test waits for `NUN Kernel Ready`, a successful frame allocation and a timer interrupt report.

## Build requirements

Required tools: GCC with x86_64 freestanding support, GNU binutils, GNU Make, GRUB utilities (`grub-file`, `grub-mkrescue`), xorriso, QEMU (`qemu-system-x86_64`) and `timeout`.

```sh
make clean iso
make qemu-test
```

The ISO is `build/nun.iso`; the kernel ELF is `build/nun.elf`; QEMU serial output is `build/qemu-serial.log`.

## Deliberate limitations

- The bootstrap maps only the first 1 GiB with identity mappings. This is a bring-up map, not the final higher-half virtual-memory layout.
- The physical allocator is an early sequential frame allocator. Buddy/slab, free lists, NUMA, COW, ASLR and user address spaces are not implemented.
- The timer is the legacy PIT behind the PIC. APIC/IOAPIC, HPET, TSC-deadline and SMP are not implemented.
- Exception entry is diagnostic-only. There is no process scheduler, userspace, syscall ABI, filesystem, network stack or device-driver framework yet.
- Only the timer IRQ is unmasked. Keyboard, USB, PCIe, ACPI, UEFI Secure Boot, ARM64 and RISC-V are outside this checkpoint.
- A passing QEMU smoke test does not qualify physical hardware or prove long-duration leak freedom.

## Acceptance evidence

The CI job is the source of truth for this phase. It must build a Multiboot2 image, boot QEMU without a reset loop and observe these serial markers:

- `NUN Kernel Ready`
- `[MM] allocated and zeroed frame`
- `[IRQ] PIT timer interrupt observed`

The 1 kHz PIT configuration is nominal; the implementation reports a one-second tick milestone but does not claim nanosecond timer accuracy.
