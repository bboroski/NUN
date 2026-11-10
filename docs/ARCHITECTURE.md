# NUN system architecture

**Architecture baseline:** 0.1  
**Implementation state:** contract prototype; no bootable kernel yet.

## 1. System boundary

NUN is a new operating-system project. It is not a launcher skin and it is not a repackaging of Android. NUNLauncher is a separate Android application that can act as a familiar home shell today and later become a client of NUN's userland service protocol.

The first engineering target is a portable, host-testable systems core. Architecture-specific startup and privileged execution are added only after the ABI, memory ownership, lifecycle and capability rules are stable.

## 2. Logical architecture

~~~text
+---------------------------------------------------------------+
| Applications: NUN Studio | File Manager | Terminal | Settings |
+---------------------------------------------------------------+
| Language platform: Fortran | Pascal | Object Pascal / RAD     |
| SAMBU tooling | compilers | runtimes | packages | debuggers   |
+---------------------------------------------------------------+
| User services: files | network | graphics | audio | devices   |
| package manager | identity | settings | diagnostics           |
+---------------------------------------------------------------+
| Service manager | IPC broker | capability broker | VFS        |
+---------------------------------------------------------------+
| Kernel: address spaces | scheduler | sync | fault isolation    |
+---------------------------------------------------------------+
| PBSC ABI | memory contract | architecture ports | drivers      |
+---------------------------------------------------------------+
| Boot chain | ARM64 / x86_64 / RISC-V64 target-specific code     |
+---------------------------------------------------------------+
~~~

This is a responsibility map, not a claim that each box already exists as executable code.

## 3. Kernel shape

The intended design is a **small capability-oriented microkernel** with most policy-heavy services in user mode.

Kernel responsibilities:
- create and isolate address spaces and execution contexts;
- dispatch IPC and validate endpoint/handle rights;
- schedule runnable threads and deliver defined synchronization primitives;
- expose a minimal clock, interrupt and memory-management boundary;
- terminate or quarantine faulted user services without silently escalating privileges.

User-mode service responsibilities:
- filesystems, network stacks, device policy, package installation, graphics composition and audio routing;
- language runtimes, compiler frontends, IDE services and application frameworks;
- health supervision and diagnostic presentation.

No driver or application receives unrestricted device access merely because it is registered as a module. Device access is granted by explicit capabilities and a board/driver policy.

## 4. Portability and target order

| Target | Intended role | Current status |
|---|---|---|
| ARM64 / AArch64 | primary phone/tablet and modern-device target | planned; no qualified board |
| x86_64 | host development, virtual-machine bring-up | planned |
| RISC-V64 | research and portability target | planned |

A CPU family is not a qualified platform. Each port needs startup code, linker map, interrupt and timer validation, memory/cache policy, device tree or equivalent hardware description, and hardware-in-the-loop evidence.

Assembly is limited to architecture entry points, context switching and hardware-specific primitives. Portable contracts remain in C11-compatible headers; higher-level services can use Rust, C, Pascal or Fortran through documented ABIs.

## 5. Service and capability model

Every service publishes:
- stable service ID and ABI major/minor;
- versioned request/response records;
- capability requirements and ownership rules;
- memory and execution budgets;
- lifecycle hooks and fault behavior;
- observable health state and verification evidence.

Handles are opaque values, not pointers and not self-authenticating secrets. The kernel's capability broker must validate ownership and rights on every privileged operation. A user-space manifest can request permissions but cannot grant them to itself.

IPC must define message size, version, cancellation, timeout, back-pressure and transport ownership. Native C struct memory is never copied to disk or sent over the network as a wire format; transports serialize fields explicitly.

## 6. Lifecycle and resilience

The service manager composes deterministic component lifecycle rules:
1. validate required capabilities and dependencies;
2. prepare caller-owned memory and resource reservations;
3. initialize in a declared order;
4. publish service endpoints only after successful initialization;
5. supervise bounded progress;
6. on failure, retain the fault cause and roll back completed stages in reverse order;
7. restart only under an explicit supervisor policy and retry budget.

CO/DLO supplies a useful lifecycle model. SaintSI/RIAE supplies bounded heartbeat observations. Neither a successful callback nor a heartbeat alone proves that a service produced correct output. CRC-32 is for accidental corruption detection, never authentication or secure boot.

## 7. Memory, timing and device rules

- Kernel and early boot code use explicitly owned memory regions; hidden allocation is disallowed in the bootstrap path.
- Each buffer has a declared length, alignment and owner.
- Interrupt handlers perform bounded work and defer non-critical processing.
- Monotonic time units and rollover behavior are part of the platform contract.
- MMIO, DMA/cache maintenance, IRQ nesting and reset semantics belong to the exact CPU/board port.
- Hardware limits come from the exact device and board revision, not generic family assumptions.

PBSC is the intended starting substrate. EFMF is an optional board-specific composition layer, not a generic substitute for a device driver.

## 8. Storage, update and trust roadmap

The planned storage stack separates block-device access, partitioning, filesystem drivers, volume policy and user-visible file APIs. The package/update system must use signed metadata and artifacts, verified trust roots, atomic activation or rollback, and an explicit recovery path.

The existing RIAE CRC implementation is not a cryptographic primitive. Secure boot, package signatures, key storage and verified updates require separately reviewed cryptographic implementations and platform trust anchors.

## 9. Build gates

- **Gate A — contract:** ABI headers, host tests, manifest and architectural review.
- **Gate B — boot:** one selected ARM64 or x86_64 development board/VM, boot log and reproducible image.
- **Gate C — kernel:** address spaces, IPC, scheduler, fault handling and target tests.
- **Gate D — services:** filesystem, process/service management, device model and security model.
- **Gate E — language platform:** compiler toolchains, runtimes, package metadata and IDE integration.
- **Gate F — shell:** native NUN shell plus Android NUNLauncher interoperability.

A gate is complete only when the relevant code, tests and evidence exist. Documentation status is not implementation status.
