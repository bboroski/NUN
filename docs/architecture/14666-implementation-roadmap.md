# NUN OS — 14-domain implementation roadmap

**Source:** user-provided “14,666 Engineering Ideas” architecture specification.
**Status:** implementation roadmap; not a claim that every listed subsystem exists.

## 1. Scope and truthfulness

The supplied catalogue names 14 domains and gives representative examples, then summarizes most intervening idea ranges by topic. It does not provide 14,666 individually testable specifications. This roadmap maps the supplied domains into engineering tracks; it does not invent descriptions for omitted entries or count roadmap rows as implemented ideas.

Every feature needs a stable ID, one canonical owner, a versioned ABI/dependency boundary, a maturity state, evidence paths, and acceptance tests. A feature is not implemented merely because it appears in a document or registry.

## 2. Fourteen engineering tracks

| # | Domain | Owner | Current position | Next gated deliverable |
|---:|---|---|---|---|
| 1 | Microkernel, scheduling, context switching | NUN | Early interrupt/timer bring-up; no preemptive scheduler | Complete interrupt frames, timer abstraction, context switch, kernel threads, scheduler tests, then SMP |
| 2 | Boot, initialization, root of trust | NUN + NUNLauncher | Multiboot2 and independent Limine paths; host-tested boot-info descriptor | Unified versioned handoff, reserved-memory validation, measured/verified boot policy, recovery-image tests |
| 3 | Memory and virtual memory | NUN | Early physical-page bitmap allocator | Reserved-region accounting, page-table builder, W^X mappings, guard pages, heap, address-space isolation |
| 4 | ABI, syscalls, interface isolation | NUN | Boot-info and host-runtime contracts; no stable user syscall ABI | Versioned syscall table, user/kernel copy validation, ELF loader, ABI conformance suite |
| 5 | Drivers and buses | NUN | Early x86_64 platform entry only | PCI enumeration, virtio-first block/console/network drivers, DMA contracts, IOMMU-gated isolation |
| 6 | Security and capability enforcement | NUN | Host-side policy components and capability design exist outside kernel enforcement | Kernel object capabilities, scoped handles, permission checks, audit trail, fault-injection tests |
| 7 | OS services and daemons | NUN | **Service dependency graph planner added in this change set**; no service execution | Process/service supervisor, bounded IPC, isolated service launch, health checks, transactional activation/rollback |
| 8 | Compilers, build tools, runtimes | SAMBU | Python-hosted language/toolkit and experimental C11 IR slice | Stable typed IR contract, native backend for a named target, reproducible builds, ABI conformance |
| 9 | Applications and package ecosystem | SAMBU + NUN | Developer tooling and project scaffolds; no NUN native app runtime | Signed package manifest, dependency resolver, capability declarations, sandbox policy, offline installation |
| 10 | Graphics, compositor, UI | NUN | No desktop/display stack in the kernel target | Framebuffer/virtio-gpu bring-up, display ABI, compositor process, window protocol, input/accessibility tests |
| 11 | Diagnostics and observability | NUN; SAMBU host tooling | Serial diagnostics and host-side engineering reports | Structured kernel events, bounded trace buffers, crash metadata, symbolized reports, privacy-safe export |
| 12 | Administration and configuration | NUN; SAMBU tooling | Existing host tools; no unified OS config service | Declarative config schema, diff/validation, atomic apply, rollback, least-privilege management API |
| 13 | Updates and software delivery | NUNLauncher + NUN | Boot-image tooling is separate from transactional OS updates | Signed manifests, staged A/B update, compatibility checks, boot confirmation, tested rollback |
| 14 | Developer tools and SDK | SAMBU | SAMBU/NUN profile contract and host-side validation being added | NUN target SDK, syscall/API bindings, cross-compile targets, emulator fixtures, debugger/profiler, docs |

## 3. Dependency order

1. **Boot and memory safety:** boot protocol validation, early console, reserved-memory map, physical allocator, page tables and kernel mapping policy.
2. **Interrupt and execution foundation:** complete exception state, interrupt-controller abstraction, timer source, context switching, kernel threads and deterministic scheduler tests.
3. **User boundary:** address spaces, user-mode transition, validated syscall ABI, executable loader and process lifecycle.
4. **Communication and services:** bounded IPC, service descriptors, service graph planner, service supervisor, isolation, capability-scoped handles and rollback.
5. **Device and storage foundation:** PCI/virtio, DMA contracts, block layer, initramfs, VFS, network stack and device permission policy.
6. **Trust and operations:** verified boot/update manifests, audit events, crash reporting, configuration transactions and recovery drills.
7. **Native developer and app ecosystem:** SAMBU target ABI, verified native code generation, package metadata, sandboxed execution and compatibility tests.
8. **Desktop and advanced optimization:** GPU/display pipeline, compositor, accessibility, measured power policy, performance tracing and workload-specific optimization.

Downstream stages may be designed in parallel, but they cannot be marked implemented before their prerequisites pass.

## 4. Cross-repository ownership

- **NUN** owns kernel entry, architecture code, memory, interrupts, scheduler, syscalls, drivers, VFS, process/service model, capability enforcement and future desktop system interfaces.
- **SAMBU** owns the language, compiler/tooling, developer SDK, profile validation and package/dependency tooling. Its Python runtime remains a host tool and must not be required during kernel boot.
- **NUNLauncher** owns future image validation and boot handoff/recovery integration. It does not replace kernel entry validation.
- Existing ecosystem components retain their repositories and API contracts. They are not merged into the kernel merely because they share a product family.

## 5. Engineering filters for speculative ideas

- **Zero latency, zero overhead, nanosecond switching, 98% savings and instant** are hypotheses, not acceptance criteria. Replace them with measured latency distributions, throughput, CPU cost, memory use, energy use and target-specific baselines.
- **AI/predictive scheduling** starts as an optional advisory policy over a deterministic baseline scheduler. It cannot override deadlines, starvation protection or safety limits without formal evaluation and fallback.
- **Post-quantum trust** must use standardized, reviewed algorithms and explicit key lifecycle/format versions. Experimental code does not establish a secure boot chain.
- **Driver isolation** requires a working process boundary, memory protection, interrupt mediation and DMA/IOMMU policy. A container label alone does not provide isolation.
- **Blockchain audit logs** are not a default kernel requirement. Prefer a bounded append-only authenticated log with explicit retention, key custody and recovery requirements unless distributed consensus is actually needed.
- **Zero-copy and lock-free paths** require ownership, lifetime, cancellation, backpressure and memory-ordering proofs. Copying is safer when boundaries matter and measured costs permit it.
- **GPU/NPU acceleration, MTE, CXL, AVX-512 and architecture-specific features** require runtime detection and feature gates. x86_64 bring-up does not imply support for AArch64, RISC-V or E2K.

## 6. Acceptance contract

A feature may move to implemented only when executable code exists in its canonical repository, its public contract is versioned, invalid input/failure/cancellation/resource exhaustion are tested, dependency and privilege requirements are explicit, repository CI passes, and target-specific evidence is attached where applicable.

Partial means an executable slice exists but the subsystem or target qualification is incomplete. Planned means no implementation is claimed. Not supported is an explicit target exclusion. The machine-readable status source is spec/os-capability-profile.json.

## 7. Delivered slice in this change set

The new nun_service_graph_plan C11 API realizes a bounded part of the declarative daemon/bootstrap concept from domain 7. It validates service IDs and dependency edges and returns a stable topological order. It does not execute services, create threads, authorize operations, or supervise failures.

SAMBU's sambu os profile check reads the capability profile and can reject SDK requests for anything not marked implemented (or partial when explicitly permitted). This gives NUN and SAMBU a concrete, testable handshake without creating a Python dependency in NUN.
