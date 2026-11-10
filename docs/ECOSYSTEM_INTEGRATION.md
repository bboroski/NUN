# NUN ecosystem integration

NUN is the integration root for seven independently maintained repositories. Every source is pinned as a Git submodule so that the complete source tree, tests, build files, assets and original repository history remain available without flattening unrelated platform code into one directory.

## Module responsibilities

| Repository | NUN responsibility | Integration boundary |
|---|---|---|
| SAMBU | Language, interpreter, CLI, IDE, editor and engineering workflows | Python host tooling; never linked into a freestanding kernel |
| ToolNG | Bounded policy, evidence assessment and workflow core | Native C policy library; fail-closed authorization |
| ReaperCripple | Bootstrap state machine, rollback, BSP, telemetry and manufacturing profiles | Native C supervision library; board-specific adapters remain explicit |
| Kloz | Platform ABI, caller-owned arena and fixed resource budget | Native C foundation |
| CO | Deterministic lifecycle state machine | Native C runtime |
| SaintSI | Heartbeats, health states and CRC-32 observation | Native C assurance; observation does not actuate hardware |
| Klipot | Ecosystem registry, contracts and integration planner | Python host control-plane tooling |

## Native runtime path

The NUN runtime composes Kloz resource limits, CO lifecycle callbacks, SaintSI health observations, ReaperCripple staged initialization/rollback and ToolNG authorization. The boot plan validates its complete step list before callbacks begin and rolls back in reverse order on failure.

All memory storage is supplied by the caller. The platform contract is versioned and must provide a monotonic tick source. The health timeout is bounded to the signed half-range of the 32-bit tick counter. The runtime does not silently create threads, write MMIO, upload data, mutate devices or enable a watchdog.

## Host tools

SAMBU and Klipot remain host-side tools. Their interpreters, portal code and planners are available inside their complete pinned submodules and are exercised by the one-knob Python entry point when pytest is installed.

## Build and verification

1. Clone NUN with recursive submodules, or run the one-knob script from a normal clone.
2. Install CMake 3.20+, a C11 compiler and Python 3.10+.
3. Install pytest for the SAMBU and Klipot Python suites.
4. Run one.py, or configure/build with CMake and run CTest manually.

The integration build compiles host-testable native libraries. It is not a bootloader, kernel image, emulator target or hardware qualification report.

## Next kernel milestone

A bootable target still requires an architecture-specific entry path, boot protocol, linker layout, early console, exception/interrupt vectors, page management, device drivers and emulator/hardware validation. Those components must be added for a named target and boot protocol; none is inferred from a host build.
