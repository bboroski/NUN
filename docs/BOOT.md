# NUN boot and installation path

## Scope

The first executable boot path targets **x86_64** and uses the Limine boot protocol.
The kernel is a freestanding ELF64 PIE. Limine supplies the firmware transition,
framebuffer information and physical memory map. NUN currently reports the boot
handoff over the first serial port (COM1) and paints a small status mark when a
usable 32-bit RGB framebuffer is available. The Limine target remains separate
from the existing Multiboot2 bring-up target, which has its own bounded early
allocator, IDT exception stubs and PIT IRQ0.

This stage is a kernel bring-up prototype. It is not yet a complete operating
system: there is no scheduler, filesystem, storage driver, network stack, package
manager, desktop environment, partition editor or persistent disk installer.

## Firmware and boot protocol

- Boot protocol: Limine, base revision 6.
- CPU target: x86_64.
- Firmware media: BIOS and x86_64 UEFI.
- Kernel format: ELF64 position-independent executable.
- Early diagnostics: COM1 serial output.
- Graphics: direct drawing to the first Limine RGB 32-bit framebuffer.
- Memory: Limine memory map is queried and counted; no allocator is enabled yet.

Limine is pinned to release 12.9.6. Its release archive is verified against the
SHA-256 digest published by the upstream GitHub Releases API before extraction.
The Limine binary redistribution notice is retained in third_party/limine/COPYING
and copied into the ISO. The protocol header is vendored from the upstream
limine-protocol project and retains its 0BSD license in
third_party/limine-protocol/LICENSE.

## Build

On Debian/Ubuntu:

    sudo apt-get update
    sudo apt-get install -y build-essential gcc clang lld nasm make curl xorriso mtools
    make iso

The generated image is build/NUN.iso; build/NUN.image.json records its SHA-256, size, x86_64 target, BIOS/UEFI firmware paths and pinned Limine release. The script builds the pinned Limine
BIOS and x86_64 UEFI components, compiles the NUN kernel, checks the ELF format
and retained Limine request section, then creates a hybrid ISO image.

## Test in a virtual machine

    make run

The kernel writes status messages to COM1. For headless test runs, connect the
emulator's serial port to the terminal and look for "NUN: boot path ready".
CI runs host-side manifest tests, then boots the same ISO in QEMU under both legacy BIOS and x86_64 UEFI (OVMF). The successful workflow publishes the ISO, kernel ELF, machine-readable image manifest and both serial logs. A green job is the evidence gate for those two emulated firmware paths; it is not a claim of support for other CPU architectures or physical machines.

## Create a bootable USB

    sudo bash scripts/install-usb.sh build/NUN.iso /dev/sdX

Replace /dev/sdX with the **whole USB disk**, not a partition such as
/dev/sdX1. The installer checks that the target is a block device, is not
read-only, is large enough, has no mounted filesystems and is not the disk
containing the running root filesystem. It prints the selected device and
requires the exact confirmation ERASE /dev/... before writing.

**This operation overwrites the selected USB disk.** Back up its contents first.
Do not select the computer's system drive. The result is boot media; it does not
install a full NUN system onto the computer's internal disk.

## Next kernel milestones

1. Connect the Limine memory-map response to the shared versioned contract in `include/nun/boot_info.h` and validate every handoff pointer before use.
2. Connect the Limine target to the existing physical-page allocation model, with reserved-range and overflow checks.
3. Add GDT/IDT, exception reporting and a structured panic screen.
4. Add block-device and GPT/ESP discovery before any persistent installer.
5. Implement a transactional installer with rollback and explicit partition plans.


## Build artifact contract

`make iso` emits `build/NUN.image.json`. NUN also has a versioned host-tested C11 boot-info contract in `include/nun/boot_info.h` and `src/boot_info.c`; it validates a caller-supplied, already mapped memory map and does not dereference physical addresses or transfer control. The current Limine entry path has not yet wired that contract into the boot handoff. The manifest identifies the exact image digest and size, the x86_64-only target, both declared firmware paths, Limine 12.9.6 and the kernel ELF ABI fields. It deliberately records the image as **unsigned**: SHA-256 detects accidental corruption but is not publisher authentication. `make test` checks manifest generation and rejects invalid kernel metadata.

GitHub Actions tests both BIOS and x86_64 UEFI via QEMU/OVMF. The downloadable artifact includes the ISO, manifest, kernel ELF and serial logs. A successful run confirms the emulator-tested x86_64 paths only; it does not certify physical hardware or any other architecture.
