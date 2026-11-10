<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" /></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-SPECIFICATION%20ONLY-FFD600?style=for-the-badge&labelColor=111111" /><img src="https://img.shields.io/badge/IMPLEMENTATION-DOCUMENTATION%20SCAFFOLD-FFD600?style=for-the-badge&labelColor=111111" /></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**A specification foundation for a minimal system kernel: architecture, memory, execution, and control-transfer boundaries.**

## Product position

NUN records the technical contract for a future minimal kernel before a full implementation exists. It gives systems engineers a shared reference for kernel responsibilities, bootstrap-provided data, and the evidence required to validate the design.

## Proposed system boundaries

- **Boot contract:** loader handoff format, memory map, and ABI version.
- **Architecture layer:** CPU entry, privilege transition, exception and interrupt vectors.
- **Memory layer:** physical pages, early allocator, virtual addressing, and protection.
- **Execution layer:** task/context model and explicit synchronization model.
- **Device boundary:** documented bus/driver interfaces without assumptions about a specific board.
- **Diagnostics:** early serial/framebuffer output and structured panic records.

## Current status

**Current status: SPECIFICATION ONLY**

The repository does not yet contain kernel source code, a boot path, linker script, build system, drivers, or tests. The README brand asset is not runtime code. NUN is not a bootable OS and must not be loaded onto physical hardware.

## Engineering requirements

Every hardware-dependent action must be tied to a specific architecture and boot protocol. ABI, memory boundaries, alignment, privilege level, and interrupt policy must be explicit. Host tests cannot replace emulator or target-platform validation.

## Project management and readiness gates

| Stage | Required evidence |
|---|---|
| Architecture baseline | CPU architecture selected and privilege model defined |
| Boot contract | Versioned handoff, memory map, and validation rules |
| Reproducible build | Build, linker map, and image checks are reproducible |
| Emulator validation | Serial smoke test, exception/interrupt, and memory tests |
| Target qualification | Physical hardware and the specific board port are validated separately |

The first implementation milestone should add a reproducible build, linker map, serial smoke test, and automated image validation. Until then, the project remains a specification scaffold.

## Contract with NUNLauncher

[NUNLauncher](https://github.com/bboroski/NUNLauncher) is intended to validate the kernel image and handoff data. NUN must independently validate the received context before initialization. Memory management, scheduling, and drivers belong to the kernel, not the launcher layer.

## Ecosystem

[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUNLauncher](https://github.com/bboroski/NUNLauncher).
