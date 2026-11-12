# NUN Next-Generation OS Blueprint

**Status:** architecture baseline / implementation plan  
**Target:** NUN x86_64 bring-up first; additional architectures only after explicit port qualification  
**Scope:** 666 requested capabilities, organized as a coherent operating-system program  
**Truth policy:** a feature is implemented only when code, tests, and target evidence exist. A design document or mockup is not implementation evidence.

## 1. Product definition

NUN is the operating-system kernel and platform integration point. The goal is a secure, local-first, AI-capable desktop and developer OS assembled from independently maintained components with versioned contracts.

The 666 requested capabilities are requirements, not claims about current functionality. The initial repository is an early x86_64 kernel bring-up: it has separate Multiboot2 and Limine paths, serial diagnostics, a limited physical-page bitmap allocator, IDT exception stubs, PIT IRQ0, and host-tested boot metadata. It does not yet have a production scheduler, SMP, ACPI, a complete driver model, VFS, networking, userspace, or a graphical shell.

## 2. System layers

1. **Boot and trust:** firmware/bootloader admission, measured boot, signed artifacts, rollback protection, key lifecycle.
2. **Kernel substrate:** CPU abstraction, interrupt routing, physical/virtual memory, scheduler, synchronization, timers, fault containment.
3. **Device and I/O:** user-mode driver boundary, DMA/IOMMU policy, storage/network/audio/display interfaces.
4. **Security and isolation:** process capabilities, sandbox policy, syscall filtering, secure IPC, secrets, audit evidence.
5. **Storage and data fabric:** VFS, block layer, snapshots, integrity, encryption, content-addressable objects, backup.
6. **Network fabric:** socket API, firewall, DNS, VPN integration, local discovery, encrypted peer transport.
7. **Runtime and compatibility:** process loader, ELF, WASM, OCI, Linux compatibility, later Windows compatibility where licensing and engineering allow.
8. **Graphics and interaction:** compositor, input, window manager, accessibility, spatial workspace, shell.
9. **Local intelligence:** model runtime, inference scheduling, semantic index, retrieval, agent sandbox and user-approved actions.
10. **Media and developer platform:** audio/video pipelines, SDK, package manager, declarative configuration, build/test tools.

## 3. Repository ownership

| Repository | Responsibility | NUN integration boundary |
|---|---|---|
| NUN | Kernel, architecture ports, memory, scheduler, syscalls, VFS and core device model | Kernel ABI, syscall ABI, boot contract |
| NUNLauncher | Boot-image admission and pre-handoff validation | Versioned image manifest and boot-info |
| Kloz | Portable bare-metal primitives and platform contracts | Versioned C ABI; no hidden global state |
| CO | Deterministic component lifecycle runtime | Lifecycle API for services and supervisors |
| SaintSI | Integrity signals, health registry and fault evidence | Event schema and health API |
| SAMBU | Language, compiler and developer workflow | Toolchain, SDK and WASM/native targets |
| ToolNG | Hardware diagnostics and service tooling | Diagnostic protocol and signed reports |
| ReaperCripple | Embedded/device-side agents and hardware adapters | Device protocol; never a kernel dependency |
| Klipot | User-facing portal and control surface | Authenticated management API |
| Luci | Integration inspection and contract validation | Read-only inspection API |
| Hellduck | Repository federation and component catalog | Pinned source references and metadata only |
| Fragile-Baby | E2K/Elbrus research and qualification | Separate architecture port and runner evidence |
| 9circ | Supporting ecosystem component; ownership to be confirmed from its source tree | No kernel-critical dependency until reviewed |

Repositories remain independent. Integration is through explicit, versioned interfaces, not source-code amalgamation or unreviewed submodule execution.

## 4. Requirement domains and allocation

The supplied 666 requirements are retained as the scope baseline and grouped by their original ranges:

| IDs | Domain | Primary owners |
|---|---|---|
| 1–80 | Kernel, memory, hardware trust, resilience | NUN, NUNLauncher, Kloz, SaintSI |
| 81–160 | Spatial desktop, shell, interaction, accessibility | NUN, Klipot, SAMBU |
| 161–240 | Local AI, semantic memory, agents, retrieval | SAMBU, NUN, Klipot |
| 241–320 | Application runtime, sandboxing, compatibility | NUN, Kloz, CO, SAMBU |
| 321–400 | Filesystems, storage, snapshots, sync | NUN, Kloz, SaintSI |
| 401–480 | Networking, mesh, firewall, communication | NUN, Kloz, ReaperCripple, ToolNG |
| 481–560 | Audio, video, graphics, creative pipeline | NUN, SAMBU, ToolNG |
| 561–666 | Developer platform, automation, diagnostics | SAMBU, ToolNG, CO, Luci, Hellduck |

A feature may have one primary owner and multiple consumers. Duplicate names in the source list must be treated as separate requirement IDs but may share one implementation when semantics and acceptance tests match.

## 5. Contracts that must exist before broad implementation

- **Boot contract:** versioned structures, architecture identifier, memory map, image bounds, entropy availability, framebuffer and ACPI table references.
- **Kernel ABI:** syscall numbering/versioning, error model, calling convention, user pointer validation and compatibility policy.
- **Memory contract:** ownership, alignment, page granularity, mapping permissions, DMA pinning and reclamation rules.
- **Device contract:** capability discovery, interrupt ownership, DMA/IOMMU requirements, reset and failure isolation.
- **Service contract:** lifecycle, health, readiness, restart policy, resource budget and shutdown deadline.
- **IPC contract:** authenticated endpoint identity, message framing, quotas, backpressure and cancellation.
- **Storage contract:** crash consistency, atomic metadata updates, integrity checks, encryption boundaries and recovery semantics.
- **Security contract:** least privilege, key custody, secure update, audit trail and explicit user consent for privileged actions.
- **AI contract:** model provenance, sandboxed inference, data locality, resource limits, citation/provenance and human approval for mutations.
- **UI contract:** accessibility, keyboard operation, reduced motion, display scaling and compositor isolation.

All public contracts require a version, compatibility statement, schema/header, conformance tests and migration notes.

## 6. Delivery sequence

### Phase 0 — Repository and evidence baseline
- Inventory actual source, tests, workflows, licenses and branch state.
- Establish a traceable requirement registry for IDs 1–666.
- Mark each item as implemented, partial, planned, blocked or not applicable with evidence links.
- Define a single architecture decision record (ADR) process and cross-repository dependency policy.

### Phase 1 — Bootable kernel foundation
- Consolidate boot metadata without silently merging the independent boot paths.
- Add architecture-neutral kernel interfaces and explicit x86_64 implementation.
- Implement memory map normalization, physical allocator tests, virtual memory mapping and page-fault diagnostics.
- Add a deterministic fixed-capacity scheduling policy core with priority, quantum, round-robin tie-breaking and optional aging; keep it host-tested until kernel context switching is ready.
- Integrate timer-driven preemption only after context-switch assembly, per-task stacks, interrupt-frame ownership and FPU/SIMD state handling are implemented and tested.
- Establish QEMU regression tests and serial-log artifacts.

### Phase 2 — Process and isolation foundation
- ELF64 loader, user/kernel transition, syscall ABI and process lifecycle.
- Capability-based handles, address-space isolation and safe copy-in/copy-out.
- First user-mode service and authenticated IPC.
- Sandbox policy and resource quotas; no claim of micro-VM isolation until a real hypervisor boundary is tested.

### Phase 3 — Storage and network
- Block-device abstraction, GPT parsing, read-only filesystem first, then writable VFS.
- Crash-consistency tests and checksummed metadata before snapshots or self-healing claims.
- Network device boundary, Ethernet, IPv4/IPv6, UDP/TCP, DNS and firewall policy.
- Encryption and P2P sync only after identity, key management and recovery contracts exist.

### Phase 4 — Drivers, graphics and desktop
- Driver isolation architecture with explicit DMA/IOMMU support requirements.
- Framebuffer/display abstraction, input stack, compositor and conventional 2D desktop.
- Window management, accessibility and workspace snapshots.
- Spatial/3D desktop is a later shell capability, not a prerequisite for a usable OS.

### Phase 5 — Applications and developer experience
- Package format, signed repository metadata, application permissions and rollback.
- WASM runtime and developer SDK before broad compatibility layers.
- OCI/container support after namespaces, cgroups-equivalent resource controls and syscall policy exist.
- Windows/macOS/Android compatibility is a separate compatibility program; it is not a universal one-click feature.

### Phase 6 — Local AI and media
- Hardware capability discovery and pluggable CPU/GPU/NPU backends.
- Local inference service with model sandbox, explicit data permissions and bounded resource use.
- Semantic index and RAG with source provenance and deletion propagation.
- Audio/video pipelines built on real device APIs and measurable latency budgets.

### Phase 7 — Advanced resilience and hardware security
- Secure/measured boot, signed updates, rollback protection and remote attestation where hardware supports it.
- PQC migration using standardized algorithms and interoperable key formats.
- Snapshot/recovery, fault injection, hardware qualification and long-duration stress testing.
- Predictive optimization only after reliable telemetry and safe fallback behavior exist.

## 7. Corrections to impossible or misleading guarantees

The following source phrases are aspirations, not valid engineering acceptance criteria:

- “Zero latency”, “zero overhead”, “instantaneous” and “zero loss” must be replaced with measurable budgets, test conditions and bounded failure behavior.
- A scheduler cannot promise literally zero scheduling delay. It can target bounded latency under a specified workload and hardware profile.
- NPU acceleration does not automatically improve kernel scheduling, memory compression or regex execution; accelerator APIs, data movement and power costs must be measured.
- Standardized post-quantum algorithms should use current NIST names: ML-KEM (formerly Kyber) and ML-DSA (formerly Dilithium). UEFI trust requires firmware/boot-chain support and key provisioning; a kernel library alone is not a hardware root of trust.
- AES-NI/ARM crypto instructions accelerate cryptography but do not themselves encrypt disks. Key management, authenticated encryption, recovery and threat modeling are required.
- Secure deletion on SSDs cannot be guaranteed by overwriting logical sectors because of wear levelling and remapping. Use device sanitize/crypto-erase where supported and report limitations.
- SSD defragmentation is generally harmful; use TRIM, filesystem-aware allocation and workload-specific maintenance.
- Deduplication must be content- and policy-aware; semantic similarity must never merge user data automatically.
- Driver isolation requires IOMMU/DMA isolation and a recovery protocol; moving a driver to userspace alone does not prevent hardware corruption.
- “Survive 30% system-file damage” is not a meaningful guarantee without redundancy, verified recovery images and a defined fault model.
- “Less than 1 ms” audio and “less than 2 ms” pen latency are end-to-end hardware targets, not promises that software can make independently.
- A cross-architecture translator is a separate runtime with ABI, syscall, graphics and licensing constraints; translation is not universal compatibility.

## 8. Security and safety invariants

1. No untrusted code runs with kernel privileges.
2. No update is activated before signature, compatibility and rollback checks pass.
3. No key is logged, exported in plaintext or embedded in build artifacts.
4. No AI agent receives implicit filesystem, network, shell or GitHub write access.
5. Destructive actions require explicit scope, preview and confirmation.
6. Recovery must preserve evidence and never silently discard user data.
7. Every security-sensitive operation emits structured, tamper-evident audit evidence.
8. Tests must include malformed inputs, resource exhaustion, fault injection and recovery paths.
9. External dependencies are pinned, license-reviewed and reproducibly fetched.
10. Claims in README and release notes must match executable evidence.

## 9. Definition of done

A requirement is complete only when all applicable evidence exists:

- implementation linked to a requirement ID;
- API/ABI and threat-model documentation;
- unit and integration tests;
- negative, fuzz or fault-injection tests where applicable;
- reproducible build and CI result;
- QEMU evidence for virtual hardware paths;
- physical hardware qualification for hardware-dependent claims;
- performance measurements with workload, machine and methodology;
- documented limitations and recovery behavior.

## 10. Immediate engineering priorities

1. Make the requirement registry machine-readable and traceable to IDs 1–666.
2. Complete the NUN subsystem ownership and dependency graph.
3. Establish kernel interfaces for memory, interrupts, timers, scheduling, processes and devices.
4. Build the first testable scheduler policy core, then integrate it with a separately tested x86_64 context-switch and user-mode transition.
5. Keep PQC, NPU, spatial desktop, broad compatibility and autonomous agents behind explicit capability gates until their prerequisites exist.

**North star:** one coherent OS, many independently testable subsystems, explicit contracts, no fabricated readiness claims.
