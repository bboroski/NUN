# NUN — Minimal Kernel Substrate

**An architectural foundation for a minimal operating-system kernel.**

Current status: the boot prototype branch adds the first executable x86_64 boot path. This is an initial kernel bring-up milestone, not a complete operating system.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md) · [Repository](https://github.com/bboroski/NUN)

## Boot and installation media

NUN uses the Limine boot protocol (base revision 6) to start an ELF64 PIE kernel on x86_64 instead of a custom, size-constrained 512-byte MBR stub. The build creates a hybrid ISO for BIOS and x86_64 UEFI. Early startup reports the handoff over COM1, queries the memory map and draws a simple splash when a compatible RGB framebuffer is available.

Debian/Ubuntu build dependencies:

    sudo apt-get update
    sudo apt-get install -y build-essential gcc clang lld nasm make curl xorriso mtools qemu-system-x86

Main commands:

    make iso
    make run
    make usb DEVICE=/dev/sdX

The ISO is written to build/NUN.iso. The USB command writes to the **entire selected disk**, not a partition. Before writing, it checks the device type, capacity, read-only status, mounted filesystems and the disk containing the running root filesystem; it then requires the exact confirmation ERASE /dev/....

**Warning:** writing overwrites the selected USB device. Back up its data first. Never select the computer's system disk.

- [Boot, build and USB installation guide](docs/BOOT.md)
- [Limine repository](https://github.com/Limine-Bootloader/Limine)
- Limine is pinned to 12.9.6; the release archive is SHA-256 verified before extraction. Its redistribution license is kept in third_party/limine/COPYING and included in the ISO.

## Current implementation boundaries

Added: early kernel entry, Limine requests for the memory map/framebuffer/bootloader information, COM1 diagnostics, a simple framebuffer splash, ISO build and guarded USB image writing.

Not yet implemented: scheduler, physical page allocator, storage drivers, filesystem, network stack, desktop environment, partition editor, Secure Boot or a persistent installer for an internal disk. The USB image is boot media for the prototype, not a full NUN installation on a computer.

## Proposed system boundaries

- **Boot contract:** loader handoff format, memory map, and ABI version.
- **Architecture layer:** CPU entry, privilege transitions, exception and interrupt vectors.
- **Memory layer:** physical pages, early allocator, virtual addressing, and protection.
- **Execution layer:** task/context model and an explicit synchronization model.
- **Device boundary:** documented bus/driver interfaces without assumptions about a specific board.
- **Capability composition layer:** capability registry, compatibility checks, composition graph, isolated admission, and atomic rollback.

## Capability composition and assimilation

NUN is specified to gain **Capability Composition & Assimilation**: a system mechanism for admitting new modules, validating their contracts and dependencies, combining compatible capabilities into composite capabilities, and reusing those composites as ordinary modules.

This is not an unrestricted merge of executable code. Each capability has a versioned manifest, stable identifier, ABI, declared interfaces, and least-privilege permissions. Admission proceeds through quarantine, validation, dependency and conflict resolution, isolated staging, and transactional activation.

- [Capability Composition & Assimilation architecture](docs/architecture/capability-composition.md)
- [Capability manifest JSON Schema](spec/capability-manifest.schema.json)

## Engineering requirements

Every hardware-dependent operation must be tied to a specific architecture and boot protocol. ABI, memory boundaries, alignment, privilege levels, and interrupt policy must be documented. Host tests cannot replace emulation or target-platform validation.

The composition system additionally requires deterministic dependency resolution, cycle rejection, module isolation, least privilege, artifact provenance, atomic activation, and verified rollback.

## Verification

A GitHub Actions workflow has been added to build the BIOS/UEFI ISO, boot it in QEMU (BIOS mode), and publish the ISO and serial log as artifacts. The build must not be considered verified until that workflow succeeds. Local commands: make iso and make run.

## Technical identity

**NUN** is the canonical technical name for the minimal kernel substrate. The GitHub path is retained only as the repository address.
