# NUN OS Implementation Roadmap

**Status:** planning baseline. No bootable image is produced by the current repository state.

## Gate N0 — Contract baseline (current slice)
- [x] Versioned boot-info C structure and validator.
- [x] Host unit tests for malformed boot-info and memory-map boundaries.
- [x] Make-based host test entry point.
- [ ] Architecture entry ABI, linker script and cross-toolchain.
- [ ] UEFI loader and signed image container.

**Exit evidence:** reproducible host tests; no hardware-support claim.

## Gate N1 — Freestanding x86_64 entry
- UEFI application loads a minimal kernel image.
- x86_64 entry assembly establishes a known stack and CPU mode.
- linker script defines explicit kernel sections and load addresses.
- early serial output and panic halt path are operational.
- kernel revalidates boot information before touching memory metadata.

**Exit evidence:** QEMU/OVMF serial boot log, linker map, image inspection, malformed-image tests and reproducible build record.

## Gate N2 — Physical memory and architecture substrate
- page-frame database with reserved-range protection;
- early allocator and Kloz integration adapter;
- page tables and virtual-memory mapping;
- GDT/IDT, exception handlers and interrupt-safe diagnostics;
- monotonic timer and architecture callback table.

**Exit evidence:** allocator property tests, fault injection, exception/interrupt tests and QEMU memory-map tests.

## Gate N3 — Execution and lifecycle
- bounded kernel tasks/threads and scheduler;
- context-switch assembly and synchronization primitives;
- CO-style lifecycle services with explicit start/step/stop behavior;
- SaintSI health observation with a monotonic timer adapter;
- finite restart and degraded-mode policies.

**Exit evidence:** scheduling invariants, race/stress tests, starvation checks, bounded shutdown and supervisor fault tests.

## Gate N4 — Device model, IPC and storage
- device descriptors and capability handles;
- interrupt routing and timer drivers;
- virtio block/network/console drivers for QEMU first;
- VFS and read-only initrd path;
- bounded IPC and service discovery.

**Exit evidence:** emulator device tests, malformed request fuzzing, mount/read failure tests and permission isolation tests.

## Gate N5 — User space and language profiles
- minimal init/supervisor, shell and system utilities;
- C ABI bindings;
- Fortran ISO_C_BINDING examples;
- Free Pascal units and runtime subset;
- Delphi source interoperability through a separately licensed compiler;
- SAMBU user-space compiler/tooling adapter.

**Exit evidence:** compile/link/run tests on the actual NUN target; explicit runtime-support matrix; negative ABI tests.

## Gate N6 — Desktop environment and package management
- window/session service and accessible system settings;
- application package manifests, signatures and dependency solver;
- staged updates and known-good rollback;
- NUNLauncher bridge on NUN builds;
- Klipot registry projection and ToolNG-inspired diagnostics.

**Exit evidence:** end-to-end install/update/rollback tests, permission review, accessibility checks and package provenance.

## Gate N7 — Additional architectures and device qualification
- AArch64 UEFI target, then selected RISC-V 64-bit systems;
- exact SoC, interrupt controller, timer, storage and display ports;
- ACPI or device-tree contract per platform;
- ReaperCripple/BSP profile and physical validation.

**Exit evidence:** named device and board revision, HIL test results, signed support matrix and recovery procedure.

## Release policy

A release must publish:
- supported architectures and exact tested profiles;
- reproducible source revision and artifact digest;
- build logs and linker map;
- automated test results and emulator/board evidence;
- known limitations and recovery instructions;
- package/image signature and rollback metadata.

Do not publish a “bootable”, “production”, “secure boot supported” or “multi-architecture supported” claim until its corresponding gate has passed.
