# NUN OS subsystem ownership and maturity

This document assigns each subsystem to one canonical repository and separates executable kernel code from host-side engineering tools. The repositories form one product ecosystem; they are not all kernel modules and must not be linked into the kernel merely because they are federated.

## Canonical ownership

| OS layer | Canonical owner | Supporting repositories | Boundary |
|---|---|---|---|
| Hardware abstraction | NUN | Kloz, ReaperCripple, Fragile-Baby | NUN owns x86_64 CPU, MMIO, interrupt and device interfaces. Kloz supplies portable platform contracts; ReaperCripple remains embedded BSP/manufacturing; Fragile-Baby is architecture research, not a production backend. |
| Boot infrastructure | NUNLauncher + NUN | Kloz, Hellduck | NUNLauncher owns image metadata validation and the future firmware handoff. NUN owns kernel entry and early initialization. Current QEMU smoke path uses GRUB Multiboot2 and does not yet execute NUNLauncher. |
| Kernel core | NUN | Kloz | NUN owns physical/virtual memory, exceptions, scheduler, threads, syscall ABI, SMP and core synchronization. Kloz does not become a global allocator or RTOS. |
| System runtime | CO, Kloz, SaintSI | NUN | CO owns deterministic component lifecycle; Kloz owns versioned platform ABI/resource primitives; SaintSI observes heartbeat and data integrity. These host-tested C libraries are not yet linked into the freestanding kernel. |
| Drivers and device services | NUN | Kloz, ReaperCripple, ToolNG | NUN owns OS drivers, device model, VFS and network stack. Kloz supplies explicit platform operations. ReaperCripple owns embedded board/firmware support. ToolNG remains a read-only Windows evidence utility. |
| System services | NUN | CO, SaintSI, ToolNG | NUN owns IPC, process/service manager, users, permissions and security enforcement. CO lifecycle and SaintSI health observations are optional service-side components, not kernel authorization. |
| SAMBU toolchain | SAMBU | NUN, Klipot, Luci | SAMBU owns language/compiler/stdlib/SDK/package tooling. A freestanding NUN target and kernel ABI binding are future work; the current Python-hosted compiler slice is not a kernel compiler. |
| Graphics and desktop | NUN (future user-space stack) | SAMBU, Klipot | NUN owns display/device interfaces and future compositor/window protocol. Applications and developer-facing tooling remain separate. No desktop is implemented by the current kernel target. |
| Developer environment | SAMBU, Klipot, Luci | ToolNG | SAMBU owns language tools; Klipot owns the federation portal; Luci owns local registry/contract checks; ToolNG owns Windows diagnostics. None is a kernel runtime. |
| Federation and updates | Hellduck | Klipot, Luci, NUNLauncher | Hellduck owns repository/release metadata and compatibility navigation. Klipot visualizes; Luci validates local manifests/policies; NUNLauncher will own boot-image validation/rollback integration. Transactional OS updates are not implemented yet. |

## Current executable milestone: x86_64 QEMU bring-up

The current feature branch adds a GRUB Multiboot2 path with:

- 32-bit loader entry and transition to x86_64 long mode;
- temporary identity mapping of physical addresses below 1 GiB using 2 MiB pages;
- COM1 serial diagnostics;
- Multiboot2 memory-map parsing and a bounded bitmap physical-page allocator;
- IDT exception stubs, PIC remapping and PIT IRQ0 at 100 Hz;
- a CI smoke test that builds an ISO, boots QEMU and checks serial output.

The pull request must pass its QEMU workflow before this is described as a verified boot. The first target is a bring-up kernel, not a complete OS.

## Ordered implementation gates

1. **Boot proof:** Multiboot2 header validation, long-mode entry, serial output, panic path, reproducible ISO and QEMU test.
2. **Memory safety:** reserve all boot-owned regions, physical allocator tests, page-table builder, kernel mapping/protection, heap and guard pages.
3. **Interrupt foundation:** complete exception frames, fault diagnostics, APIC/IOAPIC, timer abstraction and interrupt-safe synchronization.
4. **Execution:** context switch, scheduler, kernel threads, preemption, SMP bring-up and synchronization primitives.
5. **Kernel ABI:** versioned syscall table, user/kernel boundary, ELF loader and process model.
6. **Devices and storage:** ACPI, PCI/PCIe enumeration, virtio-first drivers, block layer, VFS and initramfs.
7. **Services and security:** IPC, service manager, users/groups, capability permissions, cryptographic verification and Secure Boot chain.
8. **Networking:** virtio-net, Ethernet, IPv4/IPv6, UDP/TCP, sockets and service tests.
9. **Toolchain and desktop:** SAMBU freestanding target/SDK, display driver, compositor, window protocol and shell.
10. **Updates and federation:** signed manifests, compatibility gates, A/B or transactional updates, rollback and recovery.

## Non-negotiable integration rules

- One canonical owner per subsystem; other repositories expose versioned contracts or optional adapters.
- No kernel boot dependency on Python, browser JavaScript, Windows-only tools or portal services.
- No claim of support without code, tests and a reproducible target-specific validation result.
- Host tests do not prove firmware handoff, QEMU boot or physical-hardware support.
- The kernel remains freestanding; host libraries must be explicitly audited for allocator, libc, threading and exception assumptions before any kernel linkage.
- Each milestone is merged only after its target build and regression tests pass.
