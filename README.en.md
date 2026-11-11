<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><strong>Phase A code added · QEMU verification pending</strong></p>
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

> **NUN** — NUN is a minimal system-kernel substrate with the first x86_64 bring-up implementation on a feature branch; QEMU boot is awaiting CI evidence.

## Current capabilities
- Multiboot2 x86_64 entry and transition to long mode.
- COM1 serial diagnostics and Multiboot2 memory-map validation.
- Early 4 KiB physical-frame allocator reserving the kernel and boot information.
- Initial IDT exception gates, legacy PIC remap and PIT timer IRQ.
- QEMU boot gate added; CI execution is still pending.

## Quick start and verification
Requires GCC, GNU binutils, Make, GRUB, xorriso and QEMU: `make clean iso && make qemu-test`. A production boot pipeline is not claimed.

## Architecture and verified scope
The first kernel boot path is implemented on `feature/nun-integration-phase-a`, but its QEMU smoke test is not yet confirmed by CI. This is a narrow x86_64 bring-up, not a complete OS; drivers, userspace and system services remain unimplemented.

## Interface and visual materials

NUN is currently a kernel specification foundation, not a graphical application. No GUI screenshots are available; the image below shows only the shared Duck Black / Yellow documentation style.

<p align="center"><img src="assets/duck-ui/duck-ui-preview.svg" alt="Shared Duck Black Yellow design system; not a NUN interface" width="960"></p>

## Engineering quality
- Run the checks appropriate to the target platform; host CI is not hardware qualification.
- Mark capabilities as implemented, partial or planned according to code, tests and evidence.
- A mockup, specification or open pull request is not proof of a production-ready feature.
- External integrations must be explicit, versioned, isolated and testable.

## Documentation map

- [QEMU bootstrap phase A](docs/QEMU_BOOTSTRAP.md)
- [12-repository integration map](docs/NUN_INTEGRATION.md)
- [NUN change log](CHANGELOG.NUN)
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
