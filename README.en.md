# NUN — Minimal Kernel Substrate

**A specification repository for a minimal operating-system kernel.**

Current status: **architecture documentation plus a host-integration foundation; no bootable kernel**. The repository contains specifications and a native host runtime that integrates seven source projects. An architecture-specific boot path, linker script, interrupt layer, paging and device drivers are not yet implemented.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md) · [Repository](https://github.com/bboroski/NUN)

## Proposed system boundaries

- **Boot contract:** loader handoff format, memory map, and ABI version.
- **Architecture layer:** CPU entry, privilege transitions, exception and interrupt vectors.
- **Memory layer:** physical pages, early allocator, virtual addressing, and protection.
- **Execution layer:** task/context model and an explicit synchronization model.
- **Device boundary:** documented bus/driver interfaces without assumptions about a specific board.
- **Capability composition layer:** capability registry, compatibility checks, composition graph, isolated admission, and atomic rollback.

## Capability composition and assimilation

NUN is specified to gain **Capability Composition & Assimilation**: a system mechanism for admitting new modules, validating their contracts and dependencies, combining compatible capabilities into composite capabilities, and reusing those composites as ordinary modules.

This is not an unrestricted merge of executable code. Each capability has a versioned manifest, stable identifier, ABI, declared interfaces, and least-privilege permissions. Admission proceeds through quarantine, validation, dependency and conflict resolution, isolated staging, and transactional activation.

- [Capability Composition & Assimilation architecture](docs/architecture/capability-composition.md)
- [Capability manifest JSON Schema](spec/capability-manifest.schema.json)

## Engineering requirements

Every hardware-dependent operation must be tied to a specific architecture and boot protocol. ABI, memory boundaries, alignment, privilege levels, and interrupt policy must be documented. Host tests cannot replace emulation or target-platform validation.

The composition system additionally requires deterministic dependency resolution, cycle rejection, module isolation, least privilege, artifact provenance, atomic activation, and verified rollback.

## Current verification status

The host integration build and test suites are orchestrated by one.py through CMake/CTest and Python test runners. This does not validate kernel boot: a bootable image, linker map, serial smoke test, interrupt/paging layer and target-board validation remain unimplemented.

## Integrated source ecosystem

NUN pins the complete source trees of seven independent projects as Git submodules under modules/, retaining their own histories, tests, build files and licenses.

- SAMBU — language, interpreter, CLI, IDE and developer tools; remains host-side tooling.
- ToolNG — native policy and bounded-workflow core.
- ReaperCripple — staged bootstrap, rollback and BSP adapters.
- Kloz — platform ABI, arena memory and resource budgets.
- CO — deterministic component lifecycle.
- SaintSI — health, heartbeat and integrity observation.
- Klipot — ecosystem registry, contracts and integration planner.

The native NUN runtime composes Kloz, CO, SaintSI, ReaperCripple and ToolNG: it validates the platform contract, enforces memory/queue/work budgets, starts lifecycle hooks, performs health checks and applies fail-closed authorization. SAMBU and Klipot remain available for host-side workflows in the full source tree.

Run the host integration with one.cmd on Windows or python3 one.py on Linux/macOS. See docs/ECOSYSTEM_INTEGRATION.md and ecosystem/manifest.json.

This is an integration foundation and host runtime, not a bootable OS. Architecture entry, boot protocol, linker layout, interrupt/exception vectors, paging, drivers and hardware qualification still require separate implementation.

## Technical identity

**NUN** is the canonical technical name for the minimal kernel substrate. The GitHub path is retained only as the repository address.

## Status

Do not treat this repository as a bootable OS or load it onto physical hardware: no kernel implementation exists yet.
