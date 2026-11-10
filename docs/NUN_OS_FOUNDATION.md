# NUN OS — System Architecture Foundation

**Status:** architecture baseline; boot-information validator is host-testable.  
**Initial native target:** x86_64 + UEFI + QEMU/OVMF.  
**Planned secondary targets:** AArch64 UEFI, then explicitly qualified RISC-V 64-bit platforms.  
**Product boundary:** NUN OS is a bare-metal operating system; NUNLauncher is a separate Android Home application.

## 1. Product definition

NUN is the kernel and system-software foundation for an independently bootable operating system. The target is a layered, capability-governed system with a stable C ABI, explicit hardware ports, deterministic initialization, bounded services, and evidence-backed compatibility claims.

The name of a repository, a capability manifest, or a passing host test is not evidence that a bootable operating system exists. This repository currently contains a specification and a host-side boot-information validation slice; it does not yet contain a firmware loader, kernel entry assembly, linker script, scheduler, driver set, root filesystem, or bootable image.

NUN has two distinct deployment paths:

1. **NUN OS:** firmware loads a NUN boot application, which validates and loads the kernel and transfers control using a versioned boot contract.
2. **NUNLauncher on Android:** an ordinary Android Home application. It displays and launches Android apps and may connect to NUN services only when a supported system bridge is installed and authorized.

An Android application cannot replace a device's bootloader, write protected boot partitions, or launch arbitrary bare-metal kernels on an ordinary stock device. Android-based NUN distributions require a separate AOSP/device-port and verified boot-image integration.

## 2. Layered system architecture

| Layer | Responsibility | Initial contract |
|---|---|---|
| Firmware | Platform reset, DRAM setup, firmware policy and device discovery | UEFI handoff; no firmware internals assumed by kernel |
| Boot application | Verify image, collect memory map, prepare initrd and boot information | Versioned boot-info ABI; image digest/signature policy |
| Architecture entry | CPU mode, stack, exception vectors and first kernel entry | Separate x86_64 entry implementation; explicit target feature flags |
| Kernel core | Panic path, initialization graph, capability checks and system identity | Freestanding C11 core; fixed-width types |
| Memory manager | Physical page allocator, virtual memory, mappings and protection | Reserved-region aware; checked arithmetic; no implicit heap |
| Execution | Threads/tasks, scheduler, context switching, timers and synchronization | Structured lifecycle, bounded work and explicit cancellation |
| IPC and services | Message passing, service discovery, handles and event delivery | Versioned, bounded messages; least-privilege handles |
| Device model | Bus discovery, driver binding, DMA and interrupt routing | Typed descriptors and target-owned driver adapters |
| Storage and VFS | Block devices, partition discovery, filesystems, mounts and permissions | Read-only recovery path first; writable filesystem only after fault tests |
| Security | Identity, capabilities, isolation, signature verification and audit | Fail closed; native extensions denied by default |
| User space | Init/supervisor, shell, system utilities, language runtimes and apps | Separate user/kernel address spaces |
| Desktop/session | Window manager, settings, accessibility and NUNLauncher service bridge | Independent from kernel and usable without optional network services |
| Package/update | Signed packages, compatibility metadata, staged activation and rollback | Atomic update transaction; last-known-good recovery |

Kernel code must not absorb UI, report rendering, language front ends, network dashboards, or application-specific policy. Those belong in user space and may use the shared ecosystem contracts.

## 3. Boot and initialization path

The first measurable boot target is **QEMU x86_64 with OVMF/UEFI**. The expected sequence is:

1. Firmware loads the signed or policy-approved NUN EFI application from the EFI System Partition.
2. The loader validates the kernel container, target architecture, ABI revision, payload bounds, digest and signature policy.
3. It allocates the kernel image, optional initrd, stack and a boot-information buffer.
4. It obtains the current UEFI memory map and its map key immediately before the handoff boundary.
5. It calls `ExitBootServices()`; if the map key is stale, it reacquires the map and retries using the firmware-defined rules.
6. It transfers to the architecture-specific kernel entry with the versioned boot-information pointer.
7. NUN validates the handoff again, initializes its early console, physical memory metadata, architecture tables and interrupt boundary, then enters the kernel service initialization graph.
8. The initial userspace supervisor is launched only after core invariants and required services pass their readiness checks.

After `ExitBootServices()`, the kernel must not rely on UEFI boot services. Any runtime-service use must be explicitly designed, mapped and tested; it is not an implicit dependency.

## 4. Initial target policy

The first supported engineering target is a **development target**, not a production hardware claim:

- Architecture: x86_64.
- Firmware: UEFI 2.x implementation supplied by QEMU/OVMF.
- Emulator: QEMU system emulation.
- Output: serial console first; framebuffer later.
- Image: EFI System Partition with FAT filesystem for firmware loading; a small initrd for the first userspace.
- Build: reproducible cross-toolchain, linker map, image inspection and checksum/signature verification.
- Test tiers: host unit tests → emulator boot smoke test → fault-injection tests → named physical board qualification.

AArch64 and RISC-V require separate entry code, interrupt controllers, timer ports, memory map handling, device discovery and emulator evidence. A compiler target existing does not qualify a board.

## 5. Core invariants

- All public structures are versioned and use fixed-width integer fields.
- Every memory range is checked for zero length, overflow, alignment and overlap where applicable.
- Reserved, firmware, MMIO, kernel and initrd regions are never handed to a general allocator.
- Device access is mediated by a driver and capability handle.
- Interrupt handlers do bounded work and defer non-critical work to a scheduled context.
- No untrusted module executes during discovery, parsing or signature verification.
- User-space services receive only the permissions and resources declared for them.
- A restart policy has finite retry budgets and keeps failure evidence.
- System updates stage a complete candidate image and preserve a rollback path.
- Unknown platform features remain unavailable rather than being silently emulated.

## 6. Kernel and userspace ABI

The stable boundary is a versioned NUN C ABI with:

- fixed-width status codes and structure headers;
- explicit structure size and ABI major/minor revision;
- opaque handles instead of raw device pointers;
- typed request/response messages with declared lengths;
- documented calling convention, alignment and ownership rules;
- explicit error, cancellation and timeout semantics;
- stable syscall/service identifiers, versioned independently from internal implementations.

The kernel is not required to contain a Fortran, Pascal, Delphi, Python or SAMBU runtime. Applications and language runtimes call NUN system services through the C ABI or generated language bindings.

## 7. Repository integration

NUN consumes selected, versioned contracts from the user's repositories instead of copying unrelated application code into the kernel. The integration matrix is maintained in [ECOSYSTEM_CAPABILITY_MAP.md](ECOSYSTEM_CAPABILITY_MAP.md).

- **Kloz:** caller-owned arenas, bounded resource accounting and the platform callback contract.
- **CO:** deterministic component lifecycle and one-step poll dispatch.
- **SaintSI:** health observation, monotonic heartbeat contracts and CRC-32 for accidental-corruption detection only.
- **ReaperCripple:** architecture/BSP inventory, capability-gated bootstrap, rollback model and board-profile discipline.
- **SAMBU:** systems-language/compiler architecture, developer tooling, actors/supervision concepts and structured policy descriptions.
- **ToolNG:** evidence-aware diagnostics, bounded reports and local-first UX patterns; Windows device APIs are not kernel dependencies.
- **Klipot:** capability registry, schemas, dependency planning and operator control-plane concepts.
- **NUNLauncher:** Android Home shell and, later, a permissioned service bridge to NUN user space.

Each upstream project keeps its independent release and validation process. A declared integration is not an implemented runtime dependency.

## 8. Security and update model

The default policy is deny-by-default:

- kernel image verification precedes execution;
- package provenance and content digest are retained;
- CRC-32 is not used for authentication;
- signatures are verified against pinned trust roots and an explicit revocation/rotation policy;
- anti-rollback metadata is monotonic and protected by the platform where available;
- native kernel modules are disabled until memory isolation, ABI stability, signing and recovery are implemented;
- update activation is transactional and preserves a bootable previous image;
- diagnostic export, network connectivity and device mutation require explicit capability grants.

The ecosystem capability-composition model is useful for planning and auditing modules, but a manifest is not a sandbox. Isolation must be enforced by the kernel, process boundaries and target hardware.

## 9. Completion evidence

A milestone may be called complete only when all applicable evidence exists:

- implementation and public ABI;
- positive and negative tests;
- static analysis and sanitizer results where applicable;
- reproducible build and linker map;
- emulator or hardware evidence for the target;
- security review of new privilege boundaries;
- updated user/developer documentation;
- explicit status and limitations in the capability registry.

The current boot-info validator is a host-tested contract slice. It does not demonstrate firmware loading, privilege transitions, page tables, interrupts, scheduling, drivers or a bootable NUN image.
