<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-SPECIFICATION%20ONLY-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/IMPLEMENTATION-DOCUMENTATION%20SCAFFOLD-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/LICENSE-NOT%20DECLARED-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

---

**NUN is the specification foundation for a future minimal system kernel.** The repository defines architecture, memory, execution, and handoff boundaries before a complete implementation begins.

## Target system contracts

- **Boot contract:** versioned launcher handoff, memory map, and validation rules.
- **Architecture layer:** CPU entry, privilege transition, exception, and interrupt vectors.
- **Memory layer:** physical pages, early allocator, virtual addressing, and protection.
- **Execution layer:** task/context model and explicit synchronization.
- **Device boundary:** bus/driver interfaces without assumptions about a specific board.
- **Diagnostics:** early serial/framebuffer output and structured panic records.

## Current status

NUN already contains a C runtime, module integration, a CMake host build, and host tests for the runtime, arena, platform, and related contracts. It is **not yet a bootable OS**: a complete kernel boot path, architecture entry, linker scripts, page tables, interrupt entry, finished driver model, and verified hardware boot are still absent. A manual E2K qualification workflow has been added; it requires a self-hosted runner with LCC and is not evidence of a successful target test. Windows `.exe` game support is not implemented; its target architecture is documented in [WINDOWS_PE_COMPATIBILITY.md](docs/WINDOWS_PE_COMPATIBILITY.md).

## Implementation plan and readiness gates

| Stage | Required evidence |
|---|---|
| Architecture baseline | CPU architecture, ABI, and privilege model are defined |
| Boot contract | Versioned handoff, memory map, and validation rules |
| Reproducible build | Repeatable build, linker map, and image validation |
| Emulator validation | Serial smoke test, exception/interrupt, and memory tests |
| Target qualification | Separate verification on the specific CPU and board |

The first code milestone must deliver a minimal build, linker map, serial smoke test, and automated image validation. Host tests do not replace emulator or physical-platform validation.

## NUNLauncher contract

[NUNLauncher](https://github.com/bboroski/NUNLauncher) is expected to validate the kernel image and prepare handoff data. NUN must validate the received context independently before initialization. Memory management, the scheduler, and drivers belong to the kernel, not the launcher layer.

## Ecosystem and license

Related components: [SAMBU](https://github.com/bboroski/SAMBU), [Kloz](https://github.com/bboroski/Kloz), [CO](https://github.com/bboroski/CO), [SaintSI](https://github.com/bboroski/SaintSI), [ReaperCripple](https://github.com/bboroski/ReaperCripple), and [NUNLauncher](https://github.com/bboroski/NUNLauncher).

No <code>LICENSE</code> file is present; no license is declared. Define the terms before redistribution.

---

**Built with love for systems programming and people.**