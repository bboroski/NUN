# NUN OS federation integration map

## Current checkpoint

NUN owns the kernel image, architecture entry, early memory and interrupt path. Phase A adds an x86_64 Multiboot2 bootstrap, COM1 diagnostics, a temporary first-GiB identity map, an early page-frame allocator, selected exception stubs, legacy PIC remapping and a nominal 1 kHz PIT interrupt.

The implementation is on `feature/nun-integration-phase-a`. The QEMU job is the acceptance gate. Until that job succeeds, bootability is **pending verification**, not a completed claim. The existing CMake target remains a host-side integration build and is not the kernel-image builder.

## Repository boundaries

| Repository | NUN-facing responsibility | Current evidence and boundary |
|---|---|---|
| [NUN](https://github.com/bboroski/NUN) | Kernel, HAL contracts, memory and interrupt core | Phase-A x86_64 code added on the integration branch; QEMU result pending |
| [NUNLauncher](https://github.com/bboroski/NUNLauncher) | Kernel-image metadata validation and future handoff boundary | Portable C11 host validator; not a firmware bootloader and not used as the GRUB loader in phase A |
| [Kloz](https://github.com/bboroski/Kloz) | Portable bare-metal substrate, platform-ops ABI, caller-owned memory | Host-tested C11 allocator and platform contracts; not a global kernel allocator or driver set |
| [CO](https://github.com/bboroski/CO) | Deterministic lifecycle runtime for system components | Host-tested C11 lifecycle core; no interrupt scheduler or preemption |
| [SaintSI](https://github.com/bboroski/SaintSI) | Runtime health and integrity observation contracts | Heartbeat registry and CRC-32 checks; CRC is not authentication and the module does not reset hardware |
| [ReaperCripple](https://github.com/bboroski/ReaperCripple) | Embedded BSP, staged bootstrap, rollback and target manufacturing evidence | Host-tested C11 framework and platform profiles; each target still needs its own toolchain and hardware evidence |
| [SAMBU](https://github.com/bboroski/SAMBU) | Future NUN-target language, compiler backend and SDK | Experimental Python-hosted language plus a limited native C11 compiler slice; no complete NUN code generator |
| [Klipot](https://github.com/bboroski/Klipot) | Federation registry and integration-control portal | Static registry/portal and browser-only labs; no native LSP or kernel debugger yet |
| [Luci](https://github.com/bboroski/Luci) | Engineering-domain registry and contract/evidence checks | Experimental Python/JSON control plane; not an OS service manager or package manager |
| [ToolNG](https://github.com/bboroski/ToolNG) | Optional host-side device evidence and diagnostics | Read-only Windows diagnostic utility; not a NUN kernel driver, debugger or privileged service |
| [Hellduck](https://github.com/bboroski/Hellduck) | Workspace federation, pinned sources and cross-repository compatibility metadata | Repository catalog/bootstrap only; it does not execute participant code or perform OS updates |
| [Fragile-Baby](https://github.com/bboroski/Fragile-Baby) | Separate Elbrus/E2K qualification workstream | Architecture proposal and evidence matrix; no verified E2K boot path, and no reuse of x86_64 entry assumptions |

The twelve repositories remain independently buildable and versioned. A repository is integrated only through a reviewed contract and evidence appropriate to its role; this map does not imply that every OS subsystem already exists.

## ABI and trust rules

- Phase A uses the Multiboot2 x86 handoff supplied by GRUB. The kernel checks the boot magic and parses the memory-map tag before allocating.
- The temporary allocator returns 4 KiB frames only from available memory below the current 1 GiB identity-map limit. It reserves the kernel image, Multiboot2 information block and boot modules.
- The current NUN kernel ABI is **experimental 0.x**. No syscall ABI, userspace ABI, driver ABI or stable binary module ABI is frozen.
- NUNLauncher image-header CRC is an accidental-corruption check only. It is not a signature and must not be used as an authenticity decision.
- No repository may write MMIO, alter firmware, load privileged modules or mutate devices merely because a federation entry lists it as a participant.
- Physical addresses and architecture-specific register operations belong in the architecture/HAL boundary; higher layers consume versioned, testable interfaces.

## Ordered acceptance gates

1. Build the freestanding ELF and validate the Multiboot2 header.
2. Boot the ISO in QEMU and observe `NUN Kernel Ready` on COM1.
3. Observe a valid memory map and one allocated, zeroed physical frame.
4. Observe the PIT timer interrupt milestone after interrupts are enabled.
5. Only then expand to APIC, keyboard input, richer physical/virtual memory management and scheduling.
6. Only after the kernel foundation is repeatable should userspace, VFS, networking, security services and graphics be promoted from design to implementation.

## Not yet implemented

The phase-A branch does not implement buddy/slab allocation, higher-half mapping, page-table isolation, COW, ASLR, APIC/IOAPIC, HPET, SMP, keyboard/USB/PCIe/ACPI drivers, scheduler, syscalls, POSIX, VFS, filesystems, TCP/IP, user management, Secure Boot signatures, packages, updates, GUI/compositor, ARM64 or RISC-V ports. No 24-hour stress result, >90% kernel coverage or hardware qualification is claimed.
