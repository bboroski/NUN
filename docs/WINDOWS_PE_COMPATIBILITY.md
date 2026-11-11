# NUN Windows PE compatibility architecture

## Current maturity

NUN currently provides a bounded C runtime substrate; it is not a bootable operating system and has no process model, user-mode loader, syscall ABI, graphics stack or Windows API implementation. Native execution of Windows `.exe` games is therefore **not implemented**.

Windows executable support belongs in a user-space compatibility subsystem above the NUN kernel boundary. NUNLauncher loads and validates a NUN kernel image; it must not become a Windows application launcher.

## Required layers

1. **Container and format inspection** — identify PE32/PE32+, machine type, sections, imports, relocations, subsystem, signatures and resource limits without executing the file.
2. **Architecture admission** — require an exact native ISA match or an explicitly installed translator. Intel and AMD x86-64 share the x86-64 ISA family but may expose different extensions; Apple Silicon is AArch64, not x86-64.
3. **PE loader** — validate bounds, map sections, apply relocations, resolve imports, construct process parameters, initialize TLS and define exception/unwind behavior.
4. **Win32/Win64 compatibility ABI** — provide a versioned mapping for files, handles, memory, threads, synchronization, timers, registry-like state and windowing. Unsupported APIs must fail explicitly.
5. **Graphics, audio and input bridge** — define stable user-space interfaces and capability negotiation; a successful PE load does not imply game compatibility.
6. **Isolation and lifecycle** — run with a per-title capability profile, bounded memory/CPU budgets, explicit user data directories, clean process termination and an auditable permission boundary.
7. **Launcher UX** — scan only user-selected locations, show architecture and dependencies before launch, expose compatibility profiles and preserve logs locally.

## CPU and binary policy

| PE machine family | Native target | Policy |
|---|---|---|
| x86 | x86 | Native only after process ABI exists |
| x86-64 | x86-64 (Intel / AMD) | Native only after process ABI exists |
| ARM32 | ARM32 | Native only after process ABI exists |
| ARM64 | AArch64 (including Apple Silicon at ISA level) | Native only after process ABI exists |
| Other / unknown | Any | Reject unless a separately reviewed translator is available |

RISC-V and E2K do not execute x86 Windows binaries natively. They require a separately scoped translation/JIT implementation, binary cache policy, signal/exception mapping and performance qualification. CPU-family detection in Kloz is not an instruction translator.

## Security invariants

- Static inspection precedes mapping or execution.
- Integer overflow, overlapping sections, invalid relocations and out-of-range imports are fatal.
- No automatic elevation, silent network access or implicit access to personal directories.
- The compatibility layer is not a kernel privilege boundary by itself; it must use NUN's eventual process isolation and permission model.
- A PE checksum or CRC is not a digital signature. Signed-image trust requires a cryptographic verifier and protected trust roots.
- Game compatibility is reported per title and test environment, not as a blanket promise.

## Delivery gates

1. NUN boot and user/kernel isolation work on a verified target.
2. Process, virtual memory, syscall, interrupt and filesystem contracts are versioned and tested.
3. A non-executing PE metadata parser passes a malformed-file corpus.
4. The native PE loader passes unit, fuzz and fault-injection tests.
5. A minimal user-space ABI passes API conformance tests.
6. Graphics/audio/input adapters pass title-level compatibility tests.
7. Per-title launch is enabled only after sandbox, rollback and user-consent tests pass.

Until these gates are met, NUN should describe `.exe` game support as a roadmap capability, not a current feature.
