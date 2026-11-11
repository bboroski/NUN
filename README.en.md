<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-HOST%20TESTED%20CONTRACT-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/IMPLEMENTATION-C11%20HANDOFF%20VALIDATOR-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/LICENSE-NOT%20DECLARED-FFD600?style=for-the-badge&labelColor=111111"></p>
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

The repository now contains a first host-tested C11 boot-info contract: a versioned handoff structure and validation for architecture, kernel-image bounds, entry point, and memory map. This is **not yet a bootable OS**: architecture entry code, linker script, boot path, scheduler, drivers, and bootable binary are absent. E2K is represented only as a target identifier in the contract, not as verified support.

## First implemented system contract

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

`include/nun/boot_info.h` and `src/boot_info.c` validate handoff metadata without hardware access, allocation, relocation, or transferring control to the entry point. This is a host-tested foundation, not a kernel loader.

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