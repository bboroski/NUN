# NUN Ecosystem Capability Map

**Scope:** all repositories currently connected to the NUN engineering ecosystem.  
**Rule:** reuse verified contracts and source modules through explicit adapters. Do not copy entire repositories into the kernel or turn documentation into capability claims.

## 1. Repository responsibilities

| Canonical repository | Existing responsibility observed in its source tree | NUN integration target | Boundary / maturity |
|---|---|---|---|
| `NUN` | Kernel architecture specification and capability-composition contract; now includes a host-testable boot-info validator | Kernel ABI, boot contract, memory/execution architecture, system security | Kernel and bootloader remain unimplemented |
| `NUNLauncher` | Specification scaffold; now receives an Android Home app baseline | Android user-facing shell, app catalog, settings and later service bridge | Does not own UEFI, fastboot or privileged partition writes |
| `SAMBU` | Python language/tooling, experimental C11 IR kernel, actors, supervision, channels, native compiler design | Systems language/toolchain profiles, developer tools, workflow descriptions and conformance corpus | Experimental compiler/runtime features are not a production kernel |
| `ToolNG` | Portable C17 policy/diagnostic core and Windows desktop diagnostics | Read-only local diagnostics, report schemas, evidence-state presentation and operator UI patterns | Win32/Apple USB APIs remain platform-specific; no device mutation in kernel |
| `ReaperCripple` | C11 runtime/BSP adapters, architecture detection, capability-gated bootstrap, rollback, profiles and bounded telemetry | Board-support package contract, hardware discovery and target qualification | A detected architecture or compiled adapter is not board qualification |
| `Kloz` | C11 status ABI, caller-owned arenas, fixed resource budgets and platform callbacks | Early allocator adapter, explicit resource accounting and platform contract | Not a scheduler, general heap, preemptor or sandbox |
| `CO` | C11 deterministic component lifecycle and one-callback-per-poll runtime | Kernel service/component lifecycle and bounded init/shutdown sequencing | No allocation, thread safety or hardware register ownership |
| `SaintSI` | C11 CRC-32 and bounded health/heartbeat observation | Health state, service liveness observation and accidental-corruption checks | CRC-32 is not cryptographic authentication; no automatic reset authority |
| `Klipot` | Ecosystem registry, JSON schemas, dependency planner, portal and contract visualization | System package registry, capability graph, compatibility review and operator control plane | Current portal simulations are not a privileged package manager |

## 2. Integration model

The dependency direction for the initial platform layer is:

```text
                    NUN kernel ABI
                          |
          +---------------+----------------+
          |               |                |
        Kloz              CO            SaintSI
          |               |                |
          +---------- platform adapters ---+
                          |
                    ReaperCripple BSP
                          |
                    board / SoC ports

SAMBU ---- compiler, language bindings, developer tooling
ToolNG --- local diagnostics and evidence UX
Klipot --- manifests, schema registry and dependency planning
NUNLauncher -- Android Home shell and optional NUN service client
```

This is a target architecture, not an assertion that all edges are linked in a binary today. Keep each repository independently buildable. Use versioned headers, schemas and small adapters; avoid cyclic source dependencies.

## 3. Reuse rules

### Kloz
- Reuse caller-owned arena and budget contracts for early allocations and service quotas.
- Preserve zero-ceiling-means-deny semantics and transactional reservations.
- Serialize access externally where the library requires it.
- Do not present accounting as memory protection or an allocator.

### CO
- Use the component lifecycle model for explicit startup, polling, failure and cleanup.
- Keep one bounded step per poll; callbacks must have their own execution bound.
- Do not allow application callbacks to mutate runtime internals.

### SaintSI
- Refresh heartbeats only after the required work actually completes.
- Use a single documented monotonic clock and safe tick conversion.
- Treat degraded state as an observation for policy, not automatic proof of failure.
- Keep CRC-32 separate from secure boot, signatures and authentication.

### ReaperCripple
- Adapt BSP capability descriptors and transactional initialization.
- Preserve exact board/SOC revision requirements, callback ownership and rollback evidence.
- Use target-specific device trees, ACPI/UEFI data and datasheets as hardware truth.
- Never port generic clock/voltage changes or undocumented register access into NUN.

### SAMBU
- Treat the interpreter and native IR as separate maturity levels.
- Reuse its typed IR/compiler design and developer tooling contracts for user-space language work.
- Add Fortran, Free Pascal and Delphi front ends/bindings as distinct toolchain profiles.
- Do not embed the Python interpreter in the freestanding kernel.

### ToolNG
- Reuse local-first diagnostics, bounded event history, report schemas and unknown-versus-failed evidence semantics.
- Port platform-neutral policy only; Windows-specific enumeration remains an optional host diagnostic application.
- Do not import Apple backup inspection, driver management or desktop UI into kernel space.

### Klipot
- Reuse manifest schemas, stable IDs, dependency DAGs, conflict rejection and provenance records.
- Require content digests and compatibility checks before activation.
- Portal views remain untrusted user-space presentation; they cannot grant kernel privileges.

### NUNLauncher
- Enumerate Android launchable activities through platform package APIs and only the declared package visibility scope.
- Act as an ordinary HOME/DEFAULT activity.
- Connect to NUN through an explicit, authenticated Binder/service contract only when a supported OS build provides it.
- Never claim bootloader, root, firmware-flash or kernel-handoff privileges from a normal Android app.

## 4. Shared contracts to standardize

- Capability manifest: stable ID, semantic version, kind, target ABI, provided/required interfaces, dependencies, permissions, digest, provenance and limits.
- Event envelope: producer ID, schema version, monotonic sequence, clock domain, event type, data classification and bounded payload.
- Platform descriptor: architecture, CPU features, firmware mode, memory map source, interrupt/timer provider, device-tree/ACPI source and verified board revision.
- Service ABI: fixed-width types, explicit ownership, handles, versioned errors and cancellation.
- Health record: service ID, generation, last successful progress point, deadline, fault class and bounded diagnostic context.
- Release record: source revision, compiler/toolchain identity, artifact digests, tests, target evidence and signature metadata.

## 5. Non-imported domains

Biology vocabulary packs, creative text processing, voxel visualization, Apple device diagnostics and portal presentation may remain useful applications or datasets in the wider ecosystem. They are not automatically kernel features. Their integration requires a concrete NUN service use case, a resource/security boundary and tests.

## 6. Compatibility policy

- Every repository owns its own version and release.
- Cross-repository compatibility is checked through a pinned registry snapshot and adapter tests.
- No silent last-writer-wins behavior; conflicts are rejected or resolved by an explicit adapter.
- No runtime code is admitted merely because its repository is on the allow-list.
- A failed integration can be disabled or rolled back without corrupting the previous working system.
