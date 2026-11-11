# Inter-substrate runtime profile — NUN

**Contract:** [SAMBU runtime API v1](https://github.com/bboroski/SAMBU/blob/feat/substrate-runtime-contract-v1/docs/INTERSUBSTRATE_RUNTIME_API_V1.md)  
**Status:** architecture/integration profile. Target-specific functionality remains gated by evidence.

## Ownership
Early minimal x86_64 kernel substrate with separate Multiboot2 and Limine bring-up paths.

## Existing evidence
Host-tested boot-info validation, early physical-page bitmap allocator, exception/IDT path and PIT IRQ0; QEMU smoke tests cover the documented boot profiles. The two boot paths remain separate. Full scheduler, SMP, ACPI, VFS, network stack, userspace and complete driver model are not established.

## Integration slice
Stage kernel work in dependency order: page-table/virtual-memory contract and ownership first; interrupt-safe synchronization and per-CPU state next; process/address-space model and versioned syscall ABI only after privilege transitions; then driver/resource descriptors and bounded IPC. Reuse event IDs and resource metadata for diagnostics, but keep kernel hot paths binary, fixed-width, allocation-bounded and independent of cloud control planes.

## Explicit non-goals
No microservice architecture, cloud event broker, garbage collector, generalized containers, production security boundary, or support for other ISAs is implied. A kernel descriptor must never be treated as authorization to map arbitrary physical memory.

## Acceptance tests
QEMU tests for paging boundaries, invalid mappings, syscall privilege checks, interrupt reentrancy, lock ordering, race windows, process teardown and resource cleanup. Keep each bootloader path independently tested; add SMP only with multi-CPU emulator/target evidence.

## Maturity rule
Host validation, a manifest, a simulator, or a successful unrelated target build cannot be promoted into kernel, firmware, ABI, or physical-hardware qualification. Every supported target requires its own reproducible evidence.
