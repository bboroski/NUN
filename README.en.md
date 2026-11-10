# NUN — Minimal Kernel Substrate

**NUN is an operating system with its own kernel, boot contract, system ABI and user-space environment.**

Current status: **early x86-64 boot foundation**. The repository now contains a Limine handoff, COM1 diagnostics, a framebuffer marker, an initial physical-page allocator and native contract smoke checks for Kloz, CO, SaintSI and the ReaperCripple supervisor. This is not a complete or production-ready OS: scheduler, user mode, driver model, VFS, networking, secure boot and graphical shell remain unimplemented.

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

Capability composition remains an architectural direction, not a completed runtime. The implemented boot scaffold and native contracts are documented below. Host tests and CI must be observed passing before making a readiness claim.

## Technical identity

**NUN** is the canonical technical name for the minimal kernel substrate. The GitHub path is retained only as the repository address.

## Status

This is an early boot image for development and QEMU testing. It is not production-ready and must not be installed on a primary device. Physical hardware requires separate boot-chain, driver, memory and recovery qualification.

## Implemented foundation and build

The development branch adds an x86-64 kernel ELF, checked required Limine responses, COM1 diagnostics, a black/yellow framebuffer marker, a monotonic physical-page allocator and smoke checks for integrated C11 contracts.

~~~bash
make deps
make check
make kernel
make iso
make run
~~~

Requirements: x86-64 GCC/Clang, GNU Make, curl, git, xorriso and QEMU. CI is configured for host tests, kernel compilation and ELF inspection; no passing CI result or physical-device boot is claimed until evidence exists.

## Language environment

Fortran, Pascal and Delphi/Object Pascal are planned as full user-space toolchains, runtimes/RTLs and SDKs over a stable C ABI. This includes a Fortran module/array/ISO_C_BINDING profile, Pascal units/classes/generics/exceptions and Delphi-mode compatibility/migration tooling. They do not execute in ring 0 or bypass the system ABI.

## Implementation documentation

- [System architecture](docs/ARCHITECTURE.md)
- [Repository integration](docs/ECOSYSTEM_INTEGRATION.md)
- [Fortran / Pascal / Delphi runtime](docs/LANGUAGE_RUNTIME.md)
- [Boot and build](docs/BOOT_AND_BUILD.md)
- [Security model](docs/SECURITY_MODEL.md)

## Academic biology glossary

- [500 academic terms EN–RU](data/biology/academic_terms_500.tsv)
- [Data contract and usage boundaries](docs/BIOLOGY_DOMAIN_PACK.md)

An offline reference pack for education and metadata; it does not control systems, devices or diagnostics.
