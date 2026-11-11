# NUN OS implementation roadmap

**Status:** architecture and acceptance contract; no bootable kernel is claimed by this document.
**Baseline:** NUN main is currently a host-tested runtime/specification scaffold. The root CMake build compiles C libraries and tests; it does not produce a boot image.

## Governing rule
Build bottom-up and promote a capability only when source, automated tests, and target evidence exist. Keep the host runtime separate from the freestanding kernel target. Do not label a subsystem implemented because a contract, mock, UI, or plan exists.

## Repository ownership
| Repository | Owned subsystem | Integration boundary |
|---|---|---|
| NUN | kernel core, boot contract, system integration | kernel ABI and boot-info schema |
| NUNLauncher | boot orchestration and image selection | versioned boot handoff |
| Kloz | HAL primitives, bounded memory/resource substrate | platform-ops ABI; caller-owned memory |
| CO | component lifecycle and execution orchestration | component descriptor and lifecycle ABI |
| SaintSI | syscall/API contracts, integrity and security policy | versioned syscall and capability ABI |
| ReaperCripple | platform profiles and hardware-specific adapters | platform capability descriptors |
| SAMBU | compiler, runtime, standard library and SDK | NUN target triple, object and ABI rules |
| Klipot | component registry and compatibility metadata | signed manifest schema |
| ToolNG | diagnostics, debugger and profiling | stable trace/event formats |
| Luci | build orchestration and developer workflows | reproducible build manifest |
| Hellduck | signed artifacts, channels, updates and rollback | package and update metadata |
| Fragile-Baby | isolated experiments and stress/fuzz fixtures | test-only interfaces; never a kernel dependency by default |

## Ordered delivery gates
### Phase 0 — contracts and reproducible host baseline
- Freeze architecture-neutral boot-info, platform-ops, memory-map, interrupt, syscall and component-manifest schemas.
- Record compiler versions, target triples, linker versions and pinned module revisions.
- Keep each repository's source ownership explicit; no source duplication or implicit runtime coupling.
- CI must distinguish host tests from cross-target builds and emulator qualification.

### Phase 1 — x86_64 QEMU bring-up
- Add a freestanding x86_64 kernel target independently of NUN.Runtime.
- Define the boot protocol and linker layout; validate image format before emulator launch.
- Establish COM1 serial diagnostics and emit NUN Kernel Ready only after all required early-init steps succeed.
- Install exception and interrupt descriptor tables; report faults without silently continuing.
- Parse the bootloader memory map and reserve kernel, boot-info and framebuffer regions.
- Introduce a conservative physical page allocator with overflow, alignment and bounds tests.
- Run QEMU with serial capture and fail CI on missing readiness marker, timeout, triple fault or emulator exit failure.

**Phase 1 acceptance:** reproducible kernel image; QEMU serial log contains the readiness marker; memory-map and allocator tests pass; a controlled timer interrupt is observed. This is not a claim of ARM64 support or production hardware qualification.

### Phase 2 — memory, interrupts and execution
- Page-frame allocator, page-table abstraction, kernel heap and guard-page tests.
- IDT exception coverage (#PF, #GP, #DF), PIC/APIC timer and IRQ registration.
- Kernel threads, context switch and preemption only after interrupt and stack invariants are tested.
- SMP, NUMA, COW, ASLR and real-time scheduling remain later gates.

### Phase 3 — userland and system interfaces
- Freeze SaintSI syscall ABI and versioning rules before user processes depend on it.
- Add init, process isolation, capability handles and IPC with negative/security tests.
- Add VFS and ramfs first; add disk filesystems only with corruption and recovery tests.
- Add loopback networking before virtio-net and physical NIC support.

### Phase 4 — toolchain, drivers and desktop
- Define SAMBU target/object format and SDK headers before claiming native application support.
- Bring up virtio devices and isolated driver interfaces.
- Implement framebuffer output before compositor acceleration; graphics APIs are staged behind capability checks.
- Integrate debugger, profiler and build orchestration against stable trace and symbol formats.

### Phase 5 — federation and updates
- Signed manifests, content-addressed artifacts, channel metadata and ABI compatibility checks.
- Transactional installation and A/B rollback only after verified image boot and recovery tests.
- Hellduck distributes artifacts; it does not execute arbitrary repository code.

## Non-negotiable engineering constraints
- No physical addresses outside the HAL/platform layer.
- No unchecked arithmetic in allocators, parsers or image loaders.
- Every unsafe/architecture-specific operation has a documented invariant and test strategy.
- Check and propagate all error returns; fail closed on signature or manifest validation errors.
- Do not commit generated binaries, keys, secrets or local build output.
- Do not merge with failing required checks; do not bypass repository rulesets.
- Separate claims: implemented, partial, planned, hardware-qualified.

## Validation matrix
| Gate | Required evidence |
|---|---|
| Host baseline | CMake build, CTest, sanitizers where supported |
| Kernel build | freestanding cross-compile, linker/map validation, no hosted symbols |
| QEMU boot | timeout-bounded serial log and explicit readiness marker |
| Memory | deterministic allocator tests, randomized stress and guard checks |
| Interrupts | exception self-tests and measured timer ticks |
| Reproducibility | identical artifact hashes from clean builds |
| Security | malformed boot-info, image, manifest and syscall fuzz tests |
| Release | signed metadata, ABI report, SBOM and rollback test |

Coverage percentages are reporting targets, not substitutes for test quality or target evidence. Nanosecond timer accuracy, 24-hour leak freedom and broad hardware compatibility must be measured on specified hardware; they cannot be asserted from a QEMU smoke test.

## Change protocol
Use dependency-ordered branches named feature/nun-integration-phase-X. Every implementation PR must include: scope, ABI impact, threat model, tests, emulator logs where applicable, known limitations and rollback notes. Merge only after required checks and review pass. Update CHANGELOG.NUN for externally visible ABI/API changes.