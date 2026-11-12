# NUN ecosystem integration

NUN is the integration root for the bboroski OS ecosystem. The repositories remain independently maintained products with separate source histories, licenses, APIs and release cycles. Integration is performed through explicit, versioned contracts and reviewed adapters—not by flattening every repository into the kernel or treating every repository as a boot-time dependency.

## Integration inventory

The core ecosystem contains NUN plus eleven companion repositories. Seven companion repositories are currently pinned as Git submodules under `modules/`; the other four are linked through architecture/federation metadata and are not yet vendored into the NUN checkout. A repository being listed here does not imply that its code is linked, shipped, trusted or executed by NUN.

| Repository | Canonical responsibility | NUN integration surface | Placement / status |
|---|---|---|---|
| NUN | x86_64 kernel, HAL, memory, interrupts, scheduler, syscalls, drivers, IPC, VFS and core security enforcement | Native kernel and stable kernel/user ABI | Core; early bring-up |
| NUNLauncher | Kernel-image admission, manifest/hash checks, boot-image supervision and future handoff/recovery orchestration | Host-side image verifier first; later signed handoff protocol and recovery metadata | Companion; not yet a kernel boot dependency |
| Kloz | Versioned platform ABI, caller-owned memory arenas, resource budgets and portable platform primitives | Small freestanding-compatible C contract layer after allocator/libc/threading audit | Submodule; host-tested |
| CO | Deterministic component/service lifecycle and dependency-ordered startup/shutdown | User-space service supervisor library; optional kernel callbacks only where explicitly justified | Submodule; host-tested |
| SaintSI | Health observations, heartbeats, bounded health states and integrity/CRC diagnostics | Diagnostic events and service health API; observation-only | Submodule; host-tested |
| ReaperCripple | Embedded BSP profiles, staged bootstrap, recovery/rollback state machine and manufacturing support | Board-specific adapters and recovery service; no implicit MMIO or watchdog writes | Submodule; host-tested |
| ToolNG | Read-only diagnostics, policy/evidence assessment and security workflow tooling | Host-side evidence and advisory interface; NUN kernel remains the sole runtime authorization authority | Submodule; host tooling |
| SAMBU | Language, compiler, standard library, SDK, CLI, editor/IDE and package tooling | Host developer toolchain initially; future freestanding target, NUN ABI bindings and SDK | Submodule; host tooling |
| Klipot | Integration contracts, dependency planning, repository registry and developer workflow orchestration | Host-side manifest/contract planner; emits reviewable integration plans | Submodule; host tooling |
| Luci | Local registry, contract validation, developer portal and operator-facing ecosystem UI | Optional desktop/admin application using documented NUN user-space APIs | Companion; contract-linked, not kernel code |
| Hellduck | Federation catalog, repository provenance, release metadata and compatibility navigation | Offline-capable metadata/catalog client and signed update metadata source | Companion; control-plane metadata only |
| Fragile-Baby | Architecture experiments and compatibility research | Quarantined research inputs; promotion requires a named target, license/provenance review, tests and a separate adapter | Companion; research-only |

The 9circ repository is a separate research-intake candidate, not an admitted NUN runtime dependency. It must remain outside the boot chain and shipped system image unless its purpose, license, provenance, ownership and target compatibility are explicitly reviewed and approved.

## Integration classes

1. **Kernel-resident:** only NUN-owned code that satisfies the freestanding target, kernel ABI, memory-safety and interrupt-safety requirements.
2. **Freestanding libraries:** narrow C contracts such as Kloz, and only after auditing all dependencies and compiling them for the exact target.
3. **System services:** CO lifecycle components, SaintSI health reporting and ReaperCripple recovery orchestration, isolated behind NUN's future IPC and capability model.
4. **Host-side engineering tools:** SAMBU, Klipot and ToolNG. These must not be linked into the kernel or required to boot the OS.
5. **Desktop / operator applications:** Luci and future SAMBU-built applications, running in user space after the process, graphics and permission foundations exist.
6. **Federation / update metadata:** Hellduck and NUNLauncher metadata. Metadata can inform admission and compatibility checks but cannot authorize executable code by itself.
7. **Research quarantine:** Fragile-Baby and other unqualified inputs. Research artifacts are not automatically built, loaded or shipped.

## Existing native integration slice

The current NUN integration build composes host-testable C components from Kloz, CO, SaintSI, ReaperCripple and ToolNG. SAMBU and Klipot provide host-side workflows. This is an ecosystem/runtime library slice, not proof that those libraries are linked into the freestanding kernel.

The runtime slice must preserve these rules:

- all memory storage is supplied by the caller;
- platform contracts are versioned and validate their sizes/versions;
- lifecycle startup validates the complete plan before invoking callbacks;
- failed startup rolls back completed steps in reverse order;
- health observation is bounded and does not actuate hardware;
- diagnostics and policy assessment do not grant kernel privileges;
- no implicit thread creation, MMIO writes, network upload, watchdog activation or device mutation;
- host-only dependencies remain outside the kernel build.

## Cross-repository contract requirements

Every promoted component must provide a machine-readable manifest declaring:

- stable reverse-domain component ID and semantic version;
- repository URL, immutable source commit and artifact digest;
- license identifier and provenance;
- component class and supported target/architecture;
- provided interfaces and required interfaces with version ranges;
- explicit dependencies and conflict policy;
- requested capabilities and resource budgets;
- build/test commands and target-specific validation evidence;
- signing policy and rollback compatibility where the component is updateable.

The NUN-side admission path must validate metadata and provenance before execution. Discovery and validation are non-executing. Missing, ambiguous, cyclic or incompatible dependencies fail closed. No component inherits another component's permissions merely because they are composed together.

## Runtime and security boundaries

- NUN alone owns kernel memory management, interrupt handling, scheduling, syscall dispatch, device access mediation and final authorization.
- NUNLauncher validates image metadata and coordinates host-side admission; it does not replace firmware, the bootloader or kernel entry.
- Kloz may define portable contracts but must not become a hidden global allocator or an alternate RTOS.
- CO may coordinate service lifecycle but cannot bypass NUN process isolation or permissions.
- SaintSI reports health and integrity observations; it does not autonomously reset hardware or remediate the host.
- ReaperCripple recovery actions require explicit platform adapters, bounded policy and an auditable operator/system decision.
- ToolNG findings are evidence/advice, not kernel authorization decisions.
- SAMBU, Klipot, Luci and Hellduck are not boot-critical and must not be required for an offline boot.
- Fragile-Baby and research inputs remain quarantined until separately qualified.
- Preserve each source repository's license and notices. Do not copy or relicense code as part of integration.

## Delivery sequence

1. **Inventory and provenance:** record all eleven companion repositories, ownership, licenses, immutable source refs and current integration class.
2. **Contract normalization:** define versioned component manifests, ABI compatibility rules, dependency graph validation and evidence format.
3. **Bootable kernel baseline:** prioritize a reproducible x86_64 QEMU boot, serial diagnostics, memory-map validation, physical allocator, exception/interrupt handling and panic path.
4. **Freestanding foundations:** audit and adapt Kloz contracts; keep host tests and freestanding builds separate.
5. **Service runtime:** connect CO lifecycle and SaintSI observation through a minimal NUN service/IPC boundary; add ReaperCripple only as an explicit recovery service.
6. **Image admission:** connect NUNLauncher to signed image manifests and a verifiable boot handoff; unsigned images remain clearly identified and cannot be treated as publisher-authenticated.
7. **Security evidence:** integrate ToolNG as read-only host tooling; implement actual kernel capability enforcement inside NUN, not in an external dashboard.
8. **Developer toolchain:** add SAMBU's freestanding target and NUN SDK/ABI bindings; let Klipot generate reviewable integration plans.
9. **Desktop and operator UX:** bring up user-space graphics, window/input protocols and process isolation before admitting Luci or other desktop applications.
10. **Federation and updates:** consume Hellduck compatibility metadata; implement signed, transactional updates and rollback only after image verification and recovery foundations pass.
11. **Research promotion:** qualify Fragile-Baby components individually; keep 9circ and all other research candidates out of the runtime until explicit approval.

## Build and verification

For the currently integrated host-side slice:

1. Clone NUN with recursive submodules.
2. Install CMake 3.20+, a C11 compiler and Python 3.10+.
3. Install pytest for the SAMBU and Klipot Python suites.
4. Run the repository's `one.py` entry point, or configure/build with CMake and run CTest manually.

A passing host build validates only the host-side components exercised by that build. It does not prove a bootable kernel, firmware handoff, user-space isolation, graphics, networking, update safety or physical-hardware support.

Kernel milestones require target-specific builds and reproducible emulator evidence. QEMU success does not imply hardware qualification. Each promoted integration must have its own tests and provenance record.

## Current checkout coverage

The `modules/` submodule set currently contains SAMBU, ToolNG, ReaperCripple, Kloz, CO, SaintSI and Klipot. NUNLauncher, Luci, Hellduck and Fragile-Baby are mapped above but are not yet pinned as NUN submodules. This distinction is intentional and must remain visible until their source refs, license/provenance checks, build boundaries and integration tests are reviewed.

See [OS subsystem ownership](architecture/OS_SUBSYSTEM_OWNERSHIP.md), [capability composition](architecture/capability-composition.md), [QEMU kernel bring-up](QEMU_KERNEL_BRINGUP.md), [boot image contract](BOOT.md) and the [NUNLauncher repository](https://github.com/bboroski/NUNLauncher) for the adjacent boot-image supervisor.
