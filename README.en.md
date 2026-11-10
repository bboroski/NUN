<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>NUN OS · Bare-metal Operating System Foundation</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-FOUNDATION%20IN%20PROGRESS-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/BOOT-HOST%20CONTRACT%20TESTS-FFD600?style=for-the-badge&labelColor=111111" /></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**NUN is the foundation for an independent operating system: boot contract, architecture ports, memory, task execution, drivers, isolation and user space.**

## Product architecture

NUN OS is a separate bare-metal operating system. NUNLauncher is a separate Android Home application, not a UEFI bootloader. An ordinary Android app cannot replace a device bootloader or write protected firmware partitions.

The first NUN OS engineering target is x86_64 + UEFI under QEMU/OVMF. AArch64 and RISC-V 64-bit are planned as separate ports and are not yet claimed as implemented.

## What is in the repository now

- Versioned C11 boot-information ABI v1.
- Validation of magic, ABI revision, architecture and reserved flags.
- Memory-map validation for size, count, alignment, types, overflow and overlapping ranges.
- Kernel image/initrd range and framebuffer consistency checks.
- Host tests and a GitHub Actions workflow for strict C11 compilation.
- NUN OS architecture, boot-chain, ecosystem integration map and Fortran/Pascal/Delphi language profiles.

This is **not yet a bootable OS**: the EFI loader, linker script, kernel entry, page tables, scheduler, drivers, user space and bootable image have not been implemented.

## Language subsystems

The shared system ABI will be exposed through separate binding/runtime profiles:

- **Fortran:** modules, numeric kinds, arrays, scientific computing and `ISO_C_BINDING`.
- **Pascal / Free Pascal:** units, records, sets, classes, generics, exceptions, RTTI and RTL profiles.
- **Delphi:** Object Pascal, components, properties, events, RTTI and C ABI interoperability where a licensed toolchain is available.

Compilers and runtimes are user-space subsystems; they are not loaded into the privileged kernel.

## Ecosystem

| Repository | System role |
|---|---|
| SAMBU | Languages, compilers, IDE and developer tooling |
| ReaperCripple | BSP, architecture adapters, bootstrap and rollback |
| Kloz | Arena memory, platform contracts and resource budgets |
| CO | Lifecycle and bounded execution |
| SaintSI | Health observation, heartbeat and CRC-32 |
| ToolNG | Diagnostics, evidence model and reports |
| Klipot | Module registry, schemas and dependency graph |
| NUNLauncher | Android Home Launcher and user shell |

Integration uses versioned interfaces and explicit adapters. Application and platform-specific capabilities are not automatically moved into kernel space.

## Build and test the current host contract

```bash
make check
make test
```

These commands test only the boot-info validator. They do not build an EFI loader or create a bootable image.

## Documentation

- [NUN OS foundation](docs/NUN_OS_FOUNDATION.md)
- [Boot chain and image contract](docs/BOOT_CHAIN_AND_IMAGE_CONTRACT.md)
- [Fortran, Pascal and Delphi integration](docs/LANGUAGE_RUNTIME_INTEGRATION.md)
- [All-repository capability map](docs/ECOSYSTEM_CAPABILITY_MAP.md)
- [Implementation roadmap](docs/IMPLEMENTATION_ROADMAP.md)
- [Capability composition](docs/architecture/capability-composition.md)

Architecture, bootable-image and secure-update support must be announced only after implementation and the required emulator and target-device tests have passed.
