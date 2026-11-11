<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><strong>Specification only · documentation scaffold</strong></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<sub>Updated: 2026-11-11</sub>


## Contents

- [Current capabilities](#current-capabilities)
- [Quick start and verification](#quick-start-and-verification)
- [Architecture and verified scope](#architecture-and-verified-scope)
- [Interface and visual materials](#interface-and-visual-materials)
- [Engineering quality](#engineering-quality)
- [Documentation map](#documentation-map)
- [NUN Mesh and related repositories](#nun-mesh-and-related-repositories)
- [Design and documentation](#design-and-documentation)
- [License](#license)
- [Duck Black / Yellow palette](#duck-black-yellow-palette)

> **NUN** — NUN is a specification foundation for a future minimal system kernel, defining architecture, boot, memory and execution contracts.

## Current capabilities
- Versioned boot handoff contract with NUNLauncher.
- Architecture layer for CPU entry, privilege transitions, exceptions and interrupt vectors.
- Memory model, early allocation, virtual addressing and protection requirements.
- Readiness gates for reproducible builds, emulator and target qualification.

## Quick start and verification
Review the specifications and contracts before implementation; a complete build/boot pipeline is not currently claimed.

## Architecture and verified scope
NUN is not a bootable OS. A verified kernel boot path and driver stack are not present. Failed integration checks in open PRs are not part of main and are not represented as implemented features.

## Interface and visual materials

NUN is currently a kernel specification foundation, not a graphical application. No GUI screenshots are available; the image below shows only the shared Duck Black / Yellow documentation style.

<p align="center"><img src="assets/duck-ui/duck-ui-preview.svg" alt="Shared Duck Black Yellow design system; not a NUN interface" width="960"></p>



### Versioned boot-info contract

A host-tested C11 handoff contract is also provided in `include/nun/boot_info.h` and `src/boot_info.c`. It validates the structure version, architecture identifier, kernel-image bounds, entry point, and memory map. This validates input metadata only; it does not replace a loader, transfer control to the kernel, emulator testing, or target-hardware qualification.

```sh
cmake -S . -B build -DNUN_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Engineering quality
- Run the checks appropriate to the target platform; host CI is not hardware qualification.
- Mark capabilities as implemented, partial or planned according to code, tests and evidence.
- A mockup, specification or open pull request is not proof of a production-ready feature.
- External integrations must be explicit, versioned, isolated and testable.

## Documentation map

- [Capability composition](docs/architecture/capability-composition.md)
- [Ecosystem integration](docs/ECOSYSTEM_INTEGRATION.md)
- [Technical debt and scope](docs/TECHNICAL_DEBT_REGISTER.md)

## NUN Mesh and related repositories
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher)

Each repository retains its canonical name, code, APIs, release lifecycle and ownership boundaries. Integration means versioned contracts, not automatic merging of independent runtimes.

## Design and documentation
The shared ecosystem interface direction is **Duck Black / Yellow**: dark surfaces, yellow accents, readable typography, accessible contrast, keyboard navigation and reduced-motion support. This is a visual guideline for documentation and future tools, not a claim that NUN already includes a GUI.

## License
License not declared; no LICENSE file is present.

---
**Built with care for engineering, clarity and people.**

## Duck Black / Yellow palette
The legacy badge and mark compatibility colors remain `#FFD600` (yellow) and `#111111` (near-black). The current interface canvas is `#0D0D0D`. The complete six-color palette, fourteen functional shades and RGB/HSL values are maintained in `design/duck-ui.tokens.json` (schema 3.0.0). Yellow is reserved for primary actions, focus and selection; red and cyan identify semantic error and information states.
