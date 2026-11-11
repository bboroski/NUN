# NUN — Operating System Foundation

**Status:** architecture baseline plus host-testable boot-information contract  
**Implementation status:** early foundation; not bootable  
**Canonical project identity:** NUN  
**Design system:** Duck Black / Yellow

## 1. Product definition

NUN is intended to become an independent, bootable operating system. It is not an Android skin, a desktop application, a launcher-only project, or a collection of copied repositories.

The first engineering target is a small privileged kernel with explicit architecture boundaries and capability-oriented system services. User-space services and language runtimes remain outside the privileged kernel unless a reviewed design proves that a function must execute at kernel privilege.

NUN must remain buildable without the ecosystem portal, IDE, Windows diagnostic utility, or scientific modeling tools. Those projects provide separately versioned capabilities and engineering evidence; they are not hidden kernel dependencies.

### Scope separation

| Product | Responsibility | Must not be confused with |
|---|---|---|
| NUN | Kernel, system calls, memory, task execution, IPC, security, drivers, storage and user-space service contracts | Android application or UI shell |
| NUNLauncher Android HOME | Android home screen, app discovery, search, favorites, launch intents and optional NUN service client | Firmware bootloader or replacement Android kernel |
| NUN boot path | Firmware-to-kernel image validation and handoff, implemented as a separate boot target | Android HOME activity |

An Android HOME activity runs under the Android framework and Linux/Android kernel. It cannot replace device firmware, unlock a boot chain, or load a different kernel merely by being selected as the default launcher.

## 2. System architecture

The target execution chain is:

~~~text
UEFI firmware
    |
    v
NUN boot manager / loader (separate target)
    |  image, architecture, memory-map and handoff validation
    v
NUN early kernel
    |
    +--> Kloz platform substrate
    |      fixed-width status ABI, caller-owned memory, platform operations
    |
    +--> architecture layer
    |      CPU entry, exceptions, interrupts, page tables, atomics and SMP
    |
    +--> kernel core
    |      physical/virtual memory, tasks, scheduler, IPC, syscall boundary
    |
    +--> CO lifecycle services
    +--> SaintSI health and integrity observations
    +--> ReaperCripple board/BSP profiles through explicit adapters
    |
    v
NUN user space
    +--> system service manager
    +--> VFS and storage services
    +--> device services and network stack
    +--> security and package services
    +--> GUI / window / input services
    +--> language runtimes and application packages
    |
    +--> SAMBU, Fortran, Pascal and Delphi/Object Pascal toolchains
    +--> Klipot registry and administration projection
    +--> ToolNG host-side diagnostics (not a kernel module)
~~~

The diagram is a target architecture, not a statement that these integrations or services already run.

### Privilege policy

1. Kernel code owns address-space setup, interrupt dispatch, scheduling primitives, syscall validation and low-level resource arbitration.
2. Drivers should move to isolated user-space services when the hardware and IPC model allow it.
3. A driver is admitted only for an identified device/board profile and a reviewed capability set.
4. A repository's host-tested C code is not automatically kernel-safe. It must be compiled for the target, audited for freestanding assumptions, and tested at the target boundary.
5. Python-hosted tooling, browser interfaces and desktop GUIs never enter the privileged kernel image.

## 3. How the existing repositories contribute

The integration rule is **reuse through narrow, versioned adapters**, not source-tree copying and not one giant link step.

| Repository | Existing engineering asset | NUN integration target | Boundary / current evidence |
|---|---|---|---|
| NUN | Kernel architecture, capability composition and boot ABI specifications | Owns the OS core, ABI governance and system release gates | Specification baseline plus the host-testable boot-information contract added in this branch |
| NUNLauncher | Existing boot handoff specifications; new Android HOME application foundation | Android user-facing shell and, as a separate future target, firmware boot orchestration | Android code is a starter app; firmware bootloader and real device qualification remain future work |
| Kloz | C11 fixed-width status ABI, caller-owned arena, versioned platform-ops contract, clock and IRQ/device-barrier boundaries | Early platform substrate, allocator primitives and board-operation interface | Host-tested core; target port, interrupt behavior, cache/DMA and MMIO still require board evidence |
| CO | Deterministic component lifecycle, bounded polling, explicit callback errors and partial-init cleanup | Service lifecycle and ordered user-space/early-boot component startup | Host-tested lifecycle; it is not a thread scheduler, preemptive RTOS or interrupt controller |
| SaintSI | Bounded heartbeat registry, rollover-safe timeouts and CRC-32/ISO-HDLC | Health observation, progress reports and accidental-corruption checks | Host-tested core; CRC is not authentication and observations do not actuate hardware |
| ReaperCripple | BSP adapters, board profiles, capability-gated bootstrap, reverse-order rollback, calibration and bounded telemetry | Board bring-up profiles, hardware qualification and selected device-service adapters | Host-tested/target-validation-required; no generic pinout or undocumented register access |
| SAMBU | Experimental language, interpreter, scientific/data models, workflow/policy tools, IDE and experimental typed native IR | Developer environment, declarative service configuration, diagnostics and future native language toolchain | Experimental alpha; its native compiler is incomplete and must not be presented as a production kernel compiler |
| Klipot | Machine-readable ecosystem registry, schemas, dependency planning and contract visualization | Offline-capable module catalog, package metadata projection and administration UI | Static portal/control plane; it must not become a firmware or kernel runtime dependency |
| ToolNG | Native Windows device observation, evidence-aware diagnostics, local reports and safe recommendations | External development-host and recovery-station diagnostics | Windows desktop utility; it does not run inside NUN and must not silently mutate devices |

### Dependency direction

- NUN may consume a small, audited subset of Kloz's portable C11 contracts. Kloz must not depend on NUN.
- CO and SaintSI are optional service libraries behind adapters. The kernel must still boot when optional observers are absent.
- ReaperCripple board support is selected by an exact hardware profile. No generic adapter may guess a board's electrical limits.
- SAMBU produces descriptions, developer tools and eventually compiled user programs. A workflow description is not execution evidence.
- Klipot reads published manifests and shows compatibility. The operating system must boot without Klipot.
- ToolNG remains a Windows-side diagnostic and recovery workstation utility.
- NUNLauncher Android communicates through documented Android IPC or a future NUN network/local service protocol. It never links Android framework code into the kernel.

## 4. Kernel subsystem contracts

The first system ABI should be small, fixed-width, versioned and independent from compiler-specific object layouts.

| Subsystem | Contract to define | Required proof |
|---|---|---|
| Boot | Version, architecture, image digest, memory map, reserved regions, ACPI/DTB and framebuffer references | Malformed handoff tests and emulator boot |
| CPU/architecture | Entry ABI, exception frames, interrupt policy, atomic operations and context switch | Architecture-specific tests and disassembly review |
| Physical memory | Page descriptors, usable/reserved map, allocator ownership and overflow checks | Boundary, overlap, exhaustion and corruption tests |
| Virtual memory | Address-space creation, mapping permissions, page-fault policy and isolation | Mapping and fault-injection tests |
| Execution | Task state, context, priorities, preemption policy and cancellation | Deterministic scheduler tests, stress and starvation tests |
| IPC | Typed messages, bounded queues, endpoint ownership and permission checks | Invalid payload, queue exhaustion and isolation tests |
| Storage/VFS | Mount contract, file handles, path normalization and crash-consistent metadata | Filesystem conformance and power-loss tests |
| Device model | Device identity, capabilities, driver version and resource ownership | Exact board/profile evidence |
| Security | Principled privilege boundary, process identity, capability grants, audit and revocation | Adversarial tests and independent review |
| Diagnostics | Structured error codes, crash record, trace budget and recovery path | Fault injection and recoverability tests |

No scheduler, filesystem, networking stack, GUI server, package installer or secure-boot verifier is claimed as implemented by this foundation patch.

## 5. Boot contract and first target

### Initial reference target

The proposed first reference environment is **x86_64 UEFI in QEMU/OVMF**. This is a selection for a controlled first bring-up, not a claim that the current repository already boots there.

The loader must:

1. Identify the firmware protocol and CPU architecture.
2. Validate the image format, ABI major version, declared target and image extents.
3. Validate all address arithmetic, alignment, memory-map ranges and reserved regions.
4. Record image provenance and a cryptographic image digest. A checksum alone is not image authenticity.
5. Collect ACPI or device-tree information and optional framebuffer information.
6. Finish allocations before obtaining the final UEFI memory map.
7. Obtain the current UEFI memory map and call ExitBootServices with its current map key. If the key is stale, reacquire the map and retry according to the UEFI contract.
8. Transfer control only after the handoff object and every referenced region have been validated.

After successful UEFI ExitBootServices, the loader must not assume that boot services remain available. The kernel assumes responsibility for the platform resources.

The initial C ABI is described in include/nun/boot_info.h; src/boot_info.c validates the host-visible contract. This validator does not map arbitrary physical addresses and does not verify cryptographic signatures.

### Architecture qualification matrix

| Target | Intended role | Current state | Promotion gate |
|---|---|---|---|
| x86_64 | First UEFI/QEMU reference target | Planned; no bootable image | Loader, linker map, serial output, exceptions, memory and repeatable boot tests |
| AArch64 | 64-bit ARM systems | Planned | Cross-toolchain, firmware/emulator image and target tests |
| RISC-V 64 | Open architecture and emulator target | Planned | ABI/toolchain, entry path, trap handling and emulator tests |
| Elbrus / E2K | Dedicated architecture port | Research/planning only; unverified | Real E2K compiler/linker, ABI review, firmware or boot path, emulator/board and repeatable target evidence |
| Apple, Intel and AMD processors | Platform families, not interchangeable boot profiles | Only x86_64 is selected as the first common reference | Exact CPU mode, firmware, microarchitecture and board-specific tests |

A compiler accepting a target flag is not proof that the OS can boot on that target. E2K must remain a distinct ABI/architecture port; it must not be relabeled as x86_64 because some software offers compatibility modes.

## 6. Fortran, Pascal and Delphi as first-class system languages

These languages are **application and systems SDK capabilities**, not separate kernel implementations. NUN exposes a stable C ABI and system-call surface; each compiler receives a language-specific binding and runtime adapter.

### Shared interoperability law

- Use a documented C ABI with fixed-width integers, explicit structure size/version and caller-owned buffers.
- Do not pass compiler-private strings, dynamic arrays, class instances, exceptions, RTTI blocks or module files across a foreign ABI.
- Strings cross the boundary as UTF-8 byte spans with explicit lengths.
- Errors cross the boundary as numeric status/result records. Exceptions must be caught before they cross the C boundary.
- Pointers and physical addresses are distinct types. User programs receive handles or virtual addresses, never unrestricted kernel physical addresses.
- Each toolchain and runtime is versioned independently. Compiler acceptance is not binary compatibility.

### Fortran profile

**First toolchain path:** GNU Fortran or LLVM Flang when the selected target and runtime are available.

Planned SDK features:

- MODULE-based system bindings and separately compiled numerical libraries.
- ISO_C_BINDING and BIND(C) for supported interoperable procedures and derived types.
- Explicit KIND types, arrays, allocatables, derived types and procedure interfaces.
- Numerics, vectorization and scientific libraries in user space.
- DO CONCURRENT and parallel runtimes only when the selected compiler/runtime supports the target semantics.
- Coarrays are a later distributed-memory/runtime feature; they are not a kernel primitive.

Compiler-specific .mod files are build artifacts, not a stable cross-compiler ABI. The C interoperability boundary is the compatibility surface.

### Pascal / Object Pascal profile

**First toolchain path:** Free Pascal (FPC), with a deliberately documented Object Pascal language subset.

Planned SDK features:

- Unit-based system interfaces and packages.
- Records, enumerations, sets, arrays, generics and typed callbacks.
- RTL adapters for strings, memory management, file I/O, time, exceptions and threads when supported by the runtime.
- C-compatible records and calling conventions for the kernel syscall wrapper and system libraries.
- A future component framework for NUN's own UI and service model.

Runtime-managed strings, dynamic arrays, classes and exceptions remain inside the Pascal runtime boundary unless an explicit marshaling adapter is used.

### Delphi profile

Delphi support is an **Object Pascal source/runtime compatibility workstream**, not a promise of Delphi binary compatibility.

Planned capabilities:

- Source-level compatibility targets for selected Object Pascal constructs.
- A documented subset of Delphi-style RTL APIs implemented or mapped through a compatible runtime.
- C ABI and system-service bindings independent from the Delphi compiler's private ABI.
- Optional compiler adapters where the compiler and its license are supplied by the developer.

Delphi RTL/VCL binaries and proprietary compiler components are not bundled. VCL is not the NUN kernel GUI; NUN's own window, input and component APIs are a separate system service.

### Language maturity

| Language | Initial deliverable | Current status |
|---|---|---|
| Fortran | Toolchain probe, C ABI examples, NUN.System module and numerical smoke tests | Planned |
| Pascal | FPC target profile, system units and C ABI examples | Planned |
| Delphi | Compatibility matrix, optional compiler adapter and source-level tests | Planned |
| SAMBU | Existing interpreter integration for tooling; native target only after compiler backend maturity | Experimental; native compiler incomplete |

## 7. System services and user experience

The target OS service set is modular. Initial priorities are:

1. Boot/recovery service and last-known-good image metadata.
2. Memory and task services with typed, bounded IPC.
3. Device manager with explicit driver capabilities and revocation.
4. VFS, package database and local configuration store.
5. User identity, permissions, secrets and audit log.
6. Network service with clear offline behavior.
7. Display compositor, window manager, input service and accessibility APIs.
8. Package manager and language runtime packages.
9. System settings, update manager, local help and diagnostic report viewer.

The Android HOME launcher is a companion user interface. It may display the state of a paired NUN device only after a real, authenticated service protocol exists. An offline or disconnected device must be labeled as such; mock data must never be presented as live kernel state.

## 8. Security and recovery requirements

- Capability grants are explicit, scoped, reviewable and revocable.
- Discovery and manifest parsing never execute untrusted module code.
- New services are staged and health-checked before activation.
- Composition is deterministic and atomic; unknown conflicts are rejected.
- Resource budgets cover memory, CPU time, process count, queue size, retries and diagnostic output.
- Recovery is finite. Restart loops must stop at a declared budget and preserve the original fault record.
- CRC is for accidental corruption detection. Image authenticity requires a signature scheme and protected trust anchors.
- Kernel extensions require a separate privilege, ABI, memory-safety and rollback review.
- Updates require image provenance, compatibility checks, atomic activation and a recoverable last-known-good image.

## 9. Duck Black / Yellow system design

NUN and NUNLauncher share the existing Duck design tokens rather than reintroducing unrelated palettes.

- Graphite base: #0D0D0D; panel: #1A1A1A; borders: #262626.
- Primary duck-yellow action: #FFD600; secondary accent: #BFA100.
- Primary text: #E6E6E6; secondary text: #999999; disabled text: #4D4D4D.
- Semantic error: #FF4444; success: #4CAF50; warning: yellow plus a text/icon label.
- Yellow is reserved for selected navigation, primary actions, focus, progress and high-priority states.
- Every state must remain understandable without color alone.
- Scalable typography, touch/keyboard parity, reduced motion, high contrast and readable empty/error states are mandatory.
- The same tokens must be consumed by native UI, Android resources and documentation previews.

## 10. Delivery phases and readiness gates

| Phase | Deliverable | Exit evidence |
|---|---|---|
| A — Contract baseline | Boot-info ABI, capability catalog, architecture and language boundaries | Host C tests and schema validation |
| B — UEFI loader | Reproducible x86_64 UEFI image and controlled handoff | QEMU/OVMF boot log, linker map and malformed-image rejection |
| C — Early kernel | Architecture entry, serial output, panic record and memory map adoption | Emulator smoke test and repeatable fault behavior |
| D — Memory/CPU | Physical and virtual memory, exceptions, interrupts and context switch | Boundary tests, page-fault tests and target traces |
| E — Services | IPC, process model, CO lifecycle adapter and SaintSI observer | Isolation, failure and restart-budget tests |
| F — Devices/storage | Exact BSP profiles, VFS, storage and network adapters | Device-specific and filesystem conformance tests |
| G — Language SDK | Fortran, FPC and Delphi/Object Pascal interop packs | C ABI conformance, positive/negative compiler tests |
| H — GUI and package layer | Native desktop shell, package manager, settings and recovery UI | Accessibility, persistence, rollback and security tests |
| I — Port expansion | AArch64, RISC-V and E2K ports | Independent toolchain + emulator or physical-target evidence |

No phase advances because a README or diagram mentions the feature. Each feature needs implementation, tests, failure cases, documentation and a status entry.

## 11. Current implementation truth

This branch adds a host-testable, fixed-width boot-information ABI and validator. It does **not** add a bootloader, linker script, kernel entry point, interrupt controller, scheduler, filesystem, driver, system-call dispatcher, compiler backend or bootable binary. No CPU family, including E2K, is certified by host tests.

The Android HOME starter is developed in the separate NUNLauncher repository. It is not the NUN bootloader.

## 12. External technical references

- [UEFI 2.10 Boot Services — memory map, image services and ExitBootServices](https://uefi.org/specs/UEFI/2.10/07_Services_Boot_Services.html)
- [Android Intent reference — HOME and LAUNCHER categories](https://developer.android.com/reference/android/content/Intent)
- [Android package visibility](https://developer.android.com/training/package-visibility)
- [Android LauncherApps API](https://developer.android.com/reference/android/content/pm/LauncherApps)
- [Free Pascal language, RTL and FCL documentation](https://www.freepascal.org/docs.html)
- [Free Pascal supported targets and downloads](https://www.freepascal.org/download.html)

---

**NUN — побудовано з любов'ю до системного програмування, надійності та людей.**
