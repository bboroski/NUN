# NUN — Reaper cycle model and CPU boundary

## Architectural interpretation

The Reaper-themed 6666 cycle is a bounded workload and lifecycle test vector. It may exercise scheduler budgets, timers, event recording, and deterministic state transitions after those kernel facilities exist. It is not a boot protocol, a hardware accelerator, or a new CPU architecture.

## CPU width and backends

x86-64 exposes 64-bit general-purpose registers. The model's portable state contract is expressed with uint64_t; it does not create 666-bit registers. NUN must use separate architecture backends for x86-64, AArch64, RISC-V, and Elbrus/E2K, each with its own entry convention, page-table model, interrupt ABI, and toolchain validation.

Do not infer architecture support from a shared C type or a successful host build.

## Kernel integration gates

1. Define a versioned boot-information block and architecture identifier.
2. Validate memory map, alignment, address ranges, and overflow before allocation.
3. Run the model only in an unprivileged test task after scheduler and timer initialization.
4. Bound each task's execution budget; expose cancellation and watchdog observation.
5. Record deterministic input seed, model version, phase transitions, and result checksum.
6. Keep synthetic values separate from device telemetry and physical memory maps.

## Status

This document is an architecture proposal. NUN remains a specification repository until boot code, architecture entry paths, linker scripts, emulator tests, and hardware validation exist. The reference software model is implemented separately in ReaperCripple.
