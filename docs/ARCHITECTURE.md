# NUN system architecture

**Architecture baseline: 0.1 / x86-64 bring-up.** Only components marked implemented are executable in the current foundation.

## 1. Product boundaries

| Product | Role | Not responsible for |
|---|---|---|
| NUN | bare-metal OS, kernel, system ABI, native services and user space | Android home UI |
| NUNLauncher | Android HOME launcher and optional authenticated NUN management client | PC bootloader, firmware or privileged kernel initialization |
| SAMBU | language and developer tooling | kernel privilege or automatic host execution |
| Klipot | capability registry and dependency planner | arbitrary module execution or device actuation |

A regular Android launcher cannot replace the Android kernel, unlock protected APIs or install itself as a new operating system.

## 2. Boot and privilege model

~~~text
UEFI / BIOS firmware
        |
        v
Limine boot manager (external, pinned build input)
        |
        v
NUN ELF64 kernel entry (x86-64 supervisor mode)
        |
        +--> validate boot protocol revision
        +--> serial diagnostics
        +--> framebuffer handoff
        +--> physical memory map
        +--> early page allocator
        |
        v
Kernel initialization roadmap
        |
        +--> CPU tables, exceptions and interrupt controllers
        +--> virtual memory and kernel heap
        +--> timer and scheduler
        +--> device model and drivers
        +--> VFS, storage and block cache
        +--> process loader and syscall ABI
        +--> user-space service manager
        |
        v
NUN user space: system services, language SDK, reports, package manager, shell
~~~

Initial target: x86-64 QEMU/PC, Limine protocol, ELF64 and System V x86-64 ABI. The boot protocol enters without FP/SIMD enabled; kernel code must not use floating-point/SIMD before it explicitly configures and saves that processor state.

## 3. Kernel subsystem status

| Subsystem | Contract | Status |
|---|---|---|
| Boot entry / handoff | versioned Limine requests and checked responses | implemented scaffold |
| Early diagnostics | COM1 serial and framebuffer status marker | implemented scaffold |
| Physical memory | usable memory-map regions, 4 KiB page allocation, HHDM translation | first allocator implemented |
| Kernel memory | heap, slab/cache allocator, guard pages and reclamation | planned |
| CPU/interrupts | GDT, IDT, exception records, APIC and IRQ routing | planned |
| Time | monotonic clock abstraction and rollover-safe conversions | planned |
| Scheduler | thread states, context switching, preemption and SMP locks | planned |
| Process model | ELF loader, isolated address spaces, handles and events | planned |
| Syscalls | versioned register ABI, validated user pointers and capability checks | planned |
| Device model | bus enumeration, drivers, DMA and IOMMU integration | planned |
| Storage/VFS | block devices, filesystem drivers, mount namespace and permissions | planned |
| Networking | NIC abstraction, packet buffers, network stack and sockets | planned |
| Graphics/input | display server, compositor, HID, input routing and accessibility | planned |
| Package manager | signed metadata, dependency resolution, atomic upgrades and rollback | planned |

## 4. Cross-repository service composition

~~~text
NUN kernel
  |-- Kloz          : platform ABI, arena and bounded resource accounting
  |-- CO            : deterministic component lifecycle
  |-- SaintSI       : component health observations and CRC32
  |-- ReaperCripple : optional BSP/profile bring-up and rollback
NUN user space
  |-- SAMBU         : language workbench, IDE, policies and SDK tools
  |-- ToolNG        : diagnostic evidence model and local reports
  |-- Klipot        : registry, capability catalog and dependency plan
Android device
  |-- NUNLauncher   : HOME interface, app discovery and NUN bridge client
~~~

Kloz, CO, SaintSI and the portable ReaperCripple runtime supervisor are pinned git submodules, compiled into the early kernel and exercised by bounded startup smoke checks. This is not a full integration of the upstream repositories: SAMBU, ToolNG and Klipot remain user-space capabilities, while ReaperCripple board-specific BSPs remain opt-in and require an exact board/SoC profile.

## 5. Stable NUN ABI

The first public ABI contract uses ELF64, x86-64 System V C calling convention, fixed-width integer fields, explicit structure size/version, caller-owned buffers, length/capacity pairs, numeric result codes and explicit clock units. No raw C structure is a wire format. A syscall table must validate user pointers and lengths, reject integer overflow and copy untrusted data into kernel-owned memory.

## 6. User-space service manager

Services are ordinary user processes once process isolation exists. The manager will read declarative manifests, validate schemas and dependency graphs, check resource budgets, then start allow-listed executables through a capability broker. Manifest parsing must never load a DLL, run a script, download a component or infer trust from a repository name.

Planned service families: runtime supervisor, resource governor, health observer, hardware integration, developer environment, diagnostics and registry control plane. Their exact repository ownership is documented in ECOSYSTEM_INTEGRATION.md.

## 7. Black/yellow Duck design tokens

| Token | Value | Use |
|---|---|---|
| Graphite | #0D0D0D | main background |
| Panel | #1A1A1A | cards and controls |
| Signal yellow | #FFD600 | actions, active states, focus and progress |
| Secondary yellow | #BFA100 | hover and secondary emphasis |
| Main text | #E6E6E6 | primary copy |
| Secondary text | #999999 | metadata |
| Divider | #262626 | borders |
| Error | #FF4444 | failure state |
| Success | #4CAF50 | positive completion |

Yellow is a functional indicator, not a full-surface background. Every control needs hover, pressed, disabled and visible keyboard-focus states. Layouts must respond to display scale and density; motion must never hide execution status.

## 8. Milestones

- **M0 boot proof:** reproducible ELF, validated Limine responses, serial log, page allocation smoke test and QEMU run.
- **M1 CPU foundation:** GDT/IDT, fault handling, interrupt masking and timer tests.
- **M2 memory foundation:** physical allocator map, virtual address manager, kernel heap and guard-page tests.
- **M3 execution:** context switch, cooperative scheduler, preemption and SMP locks.
- **M4 user space:** ELF loader, isolated address spaces, syscall contract and init process.
- **M5 devices/storage:** PCI, block, VFS and read-only image filesystem before write support.
- **M6 native services:** Kloz/CO/SaintSI adapters, service manager and resource governance.
- **M7 developer platform:** Fortran/Pascal/Delphi language profiles, SDK and runtime packages.
- **M8 graphical shell:** display server, input, compositor, app manager and settings.
- **M9 release qualification:** secure boot, signed updates, recovery image, fuzzing, hardware matrix and reproducible artifacts.

A milestone is complete only when code, tests, build evidence and documented limitations agree.
