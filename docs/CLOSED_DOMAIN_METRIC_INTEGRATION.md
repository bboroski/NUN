# Closed-Domain Boundary Profile — NUN

**Role:** specification scaffold for a future minimal kernel; implementation status remains specification-only until code and tests exist.

## Transfer into kernel architecture

Use explicit address-space regions and privilege boundaries as the implementable analogue of a bounded domain. Do not reinterpret these software boundaries as a physical metric field.

| CDM-1 concept | NUN specification item |
|---|---|
| Bounded region | Physical memory map, kernel image, boot-info block, stack, page tables, MMIO windows |
| Boundary | Valid address ranges, page permissions, privilege level, and ownership |
| Fixed boundary data | Versioned boot-time memory map and architecture handoff snapshot |
| Boundary stress | Page fault, guard-page violation, invalid range, overlap, and allocation exhaustion |
| Invariant ledger | Reserved/usable memory totals and region ownership checks |
| Isolation | Kernel/user address-space separation and least-privilege device access, once implemented |

## Required invariants

1. Every memory range uses overflow-safe half-open intervals `[start, end)`.
2. Reserved, usable, kernel, bootloader, stack, and MMIO regions must not overlap unless a specific shared mapping is declared.
3. Page permissions and privilege transitions are architecture-specific and must be documented per target.
4. Kernel initialization validates the handoff block independently; launcher validation is not trusted as the sole gate.
5. Unknown memory regions fail closed and are never automatically treated as usable RAM.
6. Specification, host simulation, emulator result, and physical-target result are different evidence levels.

## Acceptance gates

- Define the architecture-specific memory-map and boot-info structures.
- Add property tests for overlap, overflow, alignment, and invalid permissions.
- Add emulator tests for page-fault/guard-page and privilege-boundary behavior.
- Require a target linker map and hardware evidence before claiming a supported architecture.

## Scope

NUN remains specification-only until bootable code, build/link pipeline, and tests are present. This profile does not implement an MMU, scheduler, allocator, or bootable kernel.
