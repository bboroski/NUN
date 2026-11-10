# NUN — Minimal Kernel Substrate

**A specification repository for a minimal operating-system kernel.**

Current status: **architecture documentation only; no kernel implementation**. The repository contains README files in three languages and technical specifications. It does not yet contain kernel source, a boot path, linker script, build system, drivers, or tests. Documents describe planned architecture and do not claim that planned components are implemented.

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

There is no kernel build or test command because implementation has not started. The composition specification is an architecture contract, not a working runtime. The first code milestone should add a reproducible build, linker map, serial smoke test, image validation, and separate host tests for the registry and resolver.

## Technical identity

**NUN** is the canonical technical name for the minimal kernel substrate. The GitHub path is retained only as the repository address.

## Status

Do not treat this repository as a bootable OS or load it onto physical hardware: no kernel implementation exists yet.
