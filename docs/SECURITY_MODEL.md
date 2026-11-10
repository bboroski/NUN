# NUN security and failure-containment baseline

## Boot trust

Limine is an external boot manager and protocol provider. Pin its version and verify release artifacts. The initial foundation validates handoff responses but does not implement a complete measured-boot or secure-boot chain. Successful boot is not proof that firmware, bootloader or kernel image is trusted.

Secure-boot work requires signed kernel/module artifacts, enrolled trust anchors, anti-rollback policy, measured boot, recovery keys and a tested recovery path.

## Kernel safety

- Check every bootloader-provided pointer and count before dereferencing.
- Validate arithmetic before calculating address ranges.
- Use only memory-map entries explicitly marked usable.
- Translate physical memory through the reported HHDM offset; never hard-code the direct-map base.
- Do not access MMIO until a target-specific driver and ordering contract exist.
- Keep interrupts disabled until IDT, handlers and controller state are initialized.
- Do not enable FP/SIMD until the kernel owns and saves that processor state.
- CRC32 is not cryptographic authentication.

## Component model

Manifests are declarative data, not executable code. Validate schema, dependency graph, signature and resource requests before admitting a component. Never load native code merely because a registry lists a repository or package.

An in-process callback is not a sandbox. Untrusted extensions require process isolation, memory protection, syscall mediation and resource quotas. The early kernel does not yet provide those facilities, so no third-party code is dynamically loaded.

## Resource governance

Kloz budgets account for memory, queue slots and declared work units. They are not an allocator, preemptor, CPU timer or security boundary on their own. The kernel must enforce limits at allocator, scheduler, IPC and process layers.

## Health and recovery

SaintSI status is an observation. A fresh heartbeat is not proof of correct operation; a stale heartbeat is not a recovery instruction. Watchdog service, reset, power control and firmware update must remain explicit, separately authorized operations.

ReaperCripple hardware profiles require exact board identity and fixture evidence. Generic platform names never imply safe voltage, clock, reset or pinout values.

## User data and diagnostics

Diagnostics are local by default. Redact identifiers, paths and payloads from exported events. Use bounded logs with visible loss counters. Keep retention and export actions explicit. Recovery and update operations must be atomic or provide rollback.

## Release gates

No production claim before unit tests, QEMU boot tests, static analysis, parser fuzzing, signed artifact verification and hardware-specific qualification pass.
