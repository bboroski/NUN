<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="NUN mark" width="92"></p>
<h1 align="center">NUN</h1>
<p align="center"><strong>Minimal Kernel Substrate</strong></p>
<p align="center"><strong>Specification only · documentation scaffold</strong></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

> **NUN** — NUN is a specification foundation for a future minimal system kernel, defining architecture, boot, memory and execution contracts.

## Current capabilities
- Versioned boot handoff contract with NUNLauncher.
- Architecture layer for CPU entry, privilege transitions, exceptions and interrupt vectors.
- Memory model, early allocation, virtual addressing and protection requirements.
- Readiness gates for reproducible builds, emulator and target qualification.

## Quick start and verification
```bash
Review the specifications and contracts before implementation; a complete build/boot pipeline is not currently claimed.
```

## Architecture and verified scope
NUN is not a bootable OS. A verified kernel boot path and driver stack are not present. Failed integration checks in open PRs are not part of main and are not represented as implemented features.

## Engineering quality
- Run the checks appropriate to the target platform; host CI is not hardware qualification.
- Mark capabilities as implemented, partial or planned according to code, tests and evidence.
- A mockup, specification or open pull request is not proof of a production-ready feature.
- External integrations must be explicit, versioned, isolated and testable.

## NUN Mesh and related repositories
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher) · [Fragile-Baby](https://github.com/bboroski/Fragile-Baby) · [Luci](https://github.com/bboroski/Luci)

Each repository retains its canonical name, code, APIs, release lifecycle and ownership boundaries. Integration means versioned contracts, not automatic merging of independent runtimes.

## Design and documentation
The shared ecosystem interface direction is **Duck Black / Yellow**: dark surfaces, yellow accents, readable typography, accessible contrast, keyboard navigation and reduced-motion support. This is a visual guideline for documentation and future tools, not a claim that NUN already includes a GUI.

## License
License not declared; no LICENSE file is present.

---
**Built with care for engineering, clarity and people.**


## Duck Black / Yellow palette
The legacy badge and mark compatibility colors remain `#FFD600` (yellow) and `#111111` (near-black). The current interface canvas is `#0D0D0D`. The complete six-color palette, fourteen functional shades and RGB/HSL values are maintained in `design/duck-ui.tokens.json` (schema 3.0.0). Yellow is reserved for primary actions, focus and selection; red and cyan identify semantic error and information states.
