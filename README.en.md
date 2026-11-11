<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Operating System Foundation</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-EARLY%20FOUNDATION-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/BOOT-NOT%20BOOTABLE-FFB000?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/CORE-C11%20HOST%20CONTRACT-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<p align="center"><sub><a href="design/DUCK_UI_SYSTEM.md">Duck Black / Yellow UI standard</a> · <a href="assets/duck-ui/duck-ui-preview.svg">Vector UI preview</a></sub></p>

**NUN is the foundation for an independent operating system.** The project is moving from architecture specifications toward testable system contracts, a boot chain and a kernel.

## What exists

- Versioned C11 boot-information ABI with fixed-width fields and architecture identifiers.
- Host validator for magic, ABI, architecture, memory map, overlaps, address overflow, kernel entry, initrd and framebuffer metadata.
- Host tests for the validator and a basic make test target.
- JSON Schema for the boot manifest and a capability catalog for the nine connected repositories.
- OS architecture foundation, target-processor matrix, security/recovery policy and readiness phases.
- Integration contracts for Kloz, CO, SaintSI, ReaperCripple, SAMBU, Klipot, ToolNG and NUNLauncher.

## Status

<strong>EARLY FOUNDATION · NOT BOOTABLE</strong>

The repository does not yet contain a firmware loader, linker script, kernel entry point, interrupt controller, scheduler, virtual-memory manager, filesystem, driver stack or bootable image. Host tests validate boot metadata only; they do not prove a boot in an emulator or on physical hardware.

## Architecture

- **NUN** owns the kernel, ABI, memory, processes, IPC, security and system services.
- **Kloz** provides a narrow portable platform substrate; integration requires audit and target verification.
- **CO** and **SaintSI** can be adapted as service components but do not replace a scheduler, watchdog or cryptography.
- **ReaperCripple** provides exact BSP/board profiles and controlled bootstrap procedures.
- **SAMBU** provides experimental language and developer tooling; its native compiler is not production-ready.
- **Klipot** remains an independent control plane; **ToolNG** remains a Windows diagnostic workstation utility.

## System languages

Fortran, Pascal and Delphi/Object Pascal are planned as first-class user-space languages through a versioned C ABI, system modules, runtime adapters and libraries. Fortran uses ISO_C_BINDING/BIND(C); Pascal uses FPC and C-compatible records; Delphi has a separate source/runtime compatibility matrix. Private runtime objects do not cross the ABI boundary. SAMBU retains its own experimental toolchain.

## Target platforms

The first reference path is x86_64 UEFI under QEMU/OVMF. AArch64 and RISC-V are later ports. Elbrus/E2K remains a separate architecture workstream without verified boot evidence. Apple/Intel/AMD support is determined by the exact architecture, firmware and board, not merely by CPU vendor branding.

## Build and tests

Requires a C11 compiler and Make. Run <code>make test</code>; clean with <code>make clean</code>.

The test checks portable software invariants. It does not replace a QEMU/OVMF boot test, linker map, serial trace or physical-board qualification.

## Documentation

- [OS Foundation](docs/OS_FOUNDATION.md)
- [Boot information ABI](include/nun/boot_info.h)
- [Boot manifest schema](spec/boot-info-v1.schema.json)
- [System capability catalog](spec/system-capability-catalog.json)
- [Capability composition](docs/architecture/capability-composition.md)

## License

NUN has no LICENSE file and no declared license. Before redistribution or inclusion of code from other repositories, the license terms of each component must be reviewed independently.

---

**NUN — built with love for systems programming, reliability and people.**
