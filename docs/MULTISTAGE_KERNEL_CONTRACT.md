# NUN multi-stage kernel contract

NUN remains a specification-only repository. This document describes a staged contract and verification targets; it does not add a kernel, driver, scheduler or bootable binary.

## Kernel initialization stages

1. **Handoff intake:** validate the boot-information schema and producer version.
2. **Memory map:** reject overflow, overlap, misalignment and ranges outside physical address limits.
3. **Architecture entry:** validate CPU mode, privilege level and required instruction features.
4. **Early memory:** establish only the allocator regions proven safe by the memory map.
5. **Interrupt baseline:** install exception and interrupt tables before enabling asynchronous sources.
6. **Platform discovery:** identify only devices described by trusted platform tables.
7. **Runtime readiness:** publish a versioned capability snapshot after every mandatory stage succeeds.

Each stage has an input contract, a completion marker, a bounded resource budget and an explicit failure code. A later stage must not run when a prerequisite marker is absent. Partial startup must end in a diagnosable fault state, not a guessed fallback mode.

## Physical laws and animation boundary

Kernel code supplies monotonic time, timers, memory protection and scheduling primitives. Physics equations, rendering interpolation and animatronic trajectories belong in user-space or an independently testable domain library such as SAMBU and ReaperCripple. They must not be embedded in privileged interrupt handlers or kernel bootstrap.

## Compaction and integrity

Physical page reclamation must be handle/ownership-aware. Moving arbitrary allocations behind raw pointers is forbidden. Any future compactor needs explicit movable handles, pinned regions, relocation metadata and stop-the-world or concurrent synchronization proof. Boot markers should bind schema version, stage sequence and memory-map digest.

## Promotion gates

NUN remains SPECIFICATION ONLY until the repository contains architecture-specific entry code, linker script, reproducible build, boot image validation, emulator smoke tests and separately reported target qualification.
