# Multi-Axis Layer Architecture

**Architecture:** NUN Mesh — Multi-Axis Layer Fabric (MALF)  
**Repository:** `bboroski/NUN`  
**Role:** Minimal kernel substrate with distinct early x86_64 bring-up targets  
**Maturity:** Early x86_64 kernel bring-up; Multiboot2 and Limine paths remain independent; not a complete OS.  
**Status:** Target boundary contract, not a claim of complete OS/boot functionality.

## Purpose

MALF gives the system independent structural, vertical-slice, trust, lifecycle and evidence axes. Layers describe ownership; they are not mandatory directory names. Preserve separate bring-up paths unless an explicitly tested unification is designed.

## Structural layers

| ID | Layer | Responsibility |
|---|---|---|
| L0 | **Human and operator surface** | Operator tools, launch UI and diagnostics; not kernel privilege logic. |
| L1 | **Boot and subsystem contracts** | Boot handoff, syscall, ABI, image and subsystem data contracts. |
| L2 | **System orchestration** | Boot stages, subsystem initialization, admission and coordinated state transitions. |
| L3 | **Kernel domain** | Memory, execution, process, synchronization, fault and device models. |
| L4 | **Privilege and capability policy** | Address-space, privilege, resource and device-access authorization. |
| L5 | **Architecture-independent ports** | CPU, timer, interrupt controller, physical memory, console and device interfaces. |
| L6 | **Architecture/platform adapters** | x86_64/E2K or firmware-specific implementations, assembly entry, platform drivers. |
| L7 | **Memory and persistent state** | Memory maps, boot metadata, module tables and filesystem/storage state. |
| L8 | **Boot/runtime and delivery** | Build/link, image composition, bootloader integration, emulator and deployment. |
| L9 | **Verification and target evidence** | Host tests, boot tests, emulation, ABI checks, hardware qualification and logs. |

## Dependency and privilege law

- L0/L1 request well-defined operations; they do not bypass L4 privilege or capability checks.
- L2 owns initialization order and state transitions.
- L3 contains architecture-neutral invariants wherever practical.
- L5 is the abstraction boundary; L6 owns the machine-specific implementation.
- L7 memory ownership and persistence are explicit; no subsystem assumes a mapping or allocator that is not initialized.
- L8 composes/link-loads the image; L9 proves only the target/path actually exercised.

**Allowed:** boot/entry → validate handoff → establish minimal memory/CPU context → initialize bounded kernel subsystems → expose capability.  
**Forbidden:** user/driver path bypassing checks; unvalidated boot metadata controlling memory; unqualified target features inferred from host tests.

## Orthogonal architecture axes

1. **Structural:** L0–L9 subsystem ownership.
2. **Vertical slices:** boot, exception, memory, timer, device or image-admission path traced end-to-end.
3. **Trust:** firmware/bootloader input → validate → establish trusted execution state → capability gate → controlled operation → evidence.
4. **Lifecycle:** configure → compile/link → inspect image → boot in emulator → test → qualify hardware → release/recover.
5. **Assurance:** assertions, serial logs, test records and platform qualification remain separated by target and confidence.

## Repository-specific layer allocation

### L0 — Kernel diagnostics surface
Serial diagnostics and developer-facing boot/test entry points; no GUI or userspace claim.

### L1 — Boot and subsystem contracts
Boot handoff, memory map, architecture entry, interrupt frame and subsystem initialization contracts.

### L2 — Boot orchestration
Entry validation → minimal CPU setup → early allocator → exception/interrupt setup → selected subsystem init.

### L3 — Kernel core models
Physical page allocator, virtual address model, exceptions, synchronization and future execution/device abstractions.

### L4 — Kernel protection policy
Privilege transitions, memory-region permissions, interrupt ownership and explicit feature readiness gates.

### L5 — Architecture-neutral kernel ports
Abstract console, physical page, timer, interrupt and device operations where present.

### L6 — Architecture and boot adapters
x86_64 assembly/entry, IDT/PIT pieces and each bootloader-specific adapter. E2K userspace work stays distinct from bare-metal E2K claims.

### L7 — Memory and boot state
Boot metadata, physical page state, image manifest, module tables and future storage state.

### L8 — Kernel build and image delivery
Cross compilation, linker scripts, kernel images, ISO creation and emulator launch per boot target.

### L9 — Kernel assurance
Host manifest tests, QEMU BIOS/UEFI tests, serial evidence, boot-path-specific tests and future hardware qualification.

## Vertical slices

1. **Multiboot2 boot path:** Firmware/bootloader handoff → Multiboot2 validation → x86_64 entry → early allocator/IDT/PIT → serial evidence.
2. **Limine boot path:** Limine image composition → BIOS/UEFI boot → independent kernel entry → QEMU smoke test; do not imply shared allocator/interrupt stack until integrated.
3. **Memory initialization:** Validated memory map → reserve kernel/boot ranges → initialize physical-page bitmap → expose only initialized allocation services.

## Cross-cutting planes

- **Privilege/security:** explicit privilege transitions, memory isolation, validated pointers/ranges and least authority.
- **Fault/recovery:** defined fault states, diagnosable panic path and safe recovery policy; no silent continuation after invalid critical state.
- **Portability:** separate architecture-neutral logic from ISA/ABI/firmware-specific code.
- **Observability:** serial/diagnostic output is bounded and must not mutate state beyond the diagnostic contract.
- **Reproducibility:** pin toolchain and boot artifacts; record hashes and exact emulator/target parameters.
- **Performance:** use target-aware measurements, not extrapolation from host timings.

## NUN Mesh federation

Kernel and launcher remain separate buildable products. The launcher validates an image and handoff contract; it is not a replacement for firmware/bootloader and does not execute the kernel merely by validating it. Cross-repository contract versions are explicit; no implicit private ABI coupling.

## Incremental adoption

1. Inventory boot paths, architecture-specific code and current subsystem status.
2. Separate architecture-neutral kernel models from CPU/firmware adapters.
3. Freeze and validate boot, memory-map and image metadata contracts.
4. Gate privileged operations and every untrusted handoff field.
5. Add one vertical slice at a time with host tests, emulator evidence and target evidence kept distinct.
6. Preserve separate boot targets until there is repeatable compatibility proof.
7. Publish capability/maturity matrices with exact test paths.

## Definition of done

The entry/handoff contract is versioned; privilege and memory boundaries are explicit; architecture ports are isolated; boot and target claims have reproducible evidence; missing OS capabilities are labelled honestly.

## Repository guardrails

Do not call NUN a complete OS. Scheduler, SMP, ACPI, drivers, VFS, network, userspace and GUI must remain labelled by actual implementation status. Multiboot2 and Limine remain separate paths until explicit integration and regression tests prove otherwise. Host/QEMU success is not physical hardware qualification.
