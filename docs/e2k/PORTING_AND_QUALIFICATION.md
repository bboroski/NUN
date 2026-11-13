# Elbrus E2K portability and qualification profile

**Repository:** bboroski/NUN  
**Status:** engineering plan; implementation and target qualification are not implied.

## Project-specific adaptation
OS/native systems project with x86_64 boot-image and E2K Linux userspace verification

Keep x86_64 boot validation separate from E2K Linux userspace qualification. Add native E2K targets only for supported components; an E2K ELF check does not prove a bare-metal E2K kernel or boot path.

## Engineering rules
- Record exact CPU/stepping, board, OS/kernel, ABI, LCC version, sysroot, linker/libc, flags and source revision. Probe compiler flags; do not assume GCC flags or semantics are supported by LCC.
- Separate host build, cross-build, emulator and physical E2K execution. Host success is not target evidence.
- Optimize measured hot paths only. Inspect emitted E2K assembly/VLIW bundles and record reproducible PMU/timing data; publish no unmeasured IPC/cache/latency claims.
- Use restrict only with a true non-aliasing contract; derive alignment from target ABI/cache documentation. Do not assume 128-byte cache lines, fixed VLIW width, APB, register names or protected mode without exact manuals/toolchain evidence.
- Treat static/monolithic linking as optional. Preserve dynamic libraries, IPC, process isolation and OS services where required for security, plugins, licensing, reliability or independent updates.
- Keep a portable reference path, differential tests, bounds/overflow tests and explicit capability gates. Fast-math, unrolling and branchless transforms require semantic/numerical validation.
- Real-time claims require WCET methodology and documented scheduler, interrupt, affinity and contention assumptions. Security claims require a threat model and evidence for the exact CPU/OS/compiler mode.

## Qualification gates
| Gate | Required evidence |
|---|---|
| Inventory | Component map, languages/runtimes, dependencies and applicability decisions |
| Baseline | Reproducible host build, tests, workload and benchmark metadata |
| Toolchain | Exact LCC/sysroot/ABI manifest plus compile/link smoke test |
| Native slice | Portable hot-path component, host reference and E2K differential tests |
| Optimization | Assembly inspection and repeatable target measurements |
| Integration | Optional native backend, versioned adapter and preserved fallback |
| Hardware | Physical target identity, logs, hashes, correctness and performance evidence |

Each benchmark records hardware, OS/kernel, compiler/linker, flags, commit, artifact SHA-256, input hash, repetitions, timing distribution, correctness and execution class (host/emulator/physical E2K). CI must mark unavailable E2K execution **not run**, never pass.

## Claims boundary
This document does not assert that the application is already native, that all code should become one binary, or that any performance/security improvement has been measured. Compiler flags such as -O4, -march=native, -mtune=native, -flto and -fwhole-program require toolchain probes. APB, predication, vector widths, register windows and protected-pointer modes require exact target confirmation. Documentation-only work is **planned/specification**, not **implemented**.

## Definition of done
Tie every implementation claim to source files, reproducible builds, tests, generated-code inspection where relevant, a portable fallback and evidence appropriate to its maturity.