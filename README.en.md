# NUN — integrated systems foundation

NUN coordinates seven source repositories as one verifiable engineering system. Every repository is pinned as a Git submodule, preserving its complete source code, tests, build configuration, assets and independent history.

## Included projects

- SAMBU — language runtime, interpreter, CLI, IDE and developer tooling.
- ToolNG — native policy, evidence and bounded-workflow core.
- ReaperCripple — staged bootstrap, rollback, BSP adapters, telemetry and manufacturing profiles.
- Kloz — platform ABI, caller-owned arena and resource budgets.
- CO — deterministic component lifecycle.
- SaintSI — heartbeat, health observation and CRC-32.
- Klipot — ecosystem registry, contracts and integration planner.

## Integrated runtime

The native NUN runtime composes Kloz resources, CO lifecycle callbacks, SaintSI health observations, ReaperCripple bootstrap/rollback and ToolNG fail-closed authorization. It validates a versioned platform contract, uses caller-owned memory, bounds memory/queue/work and rolls back bootstrap stages in reverse order.

SAMBU and Klipot remain host-side tools. Their complete Python code is available in the pinned submodules and their test suites are included in the one-knob workflow when pytest is installed. Host Python code is not presented as freestanding kernel code.

## One-knob build

Clone with recursive submodules, then run one.cmd on Windows or python3 one.py on Linux/macOS. Requirements: CMake 3.20+, a C11 compiler and Python 3.10+. Install pytest to execute the SAMBU and Klipot test suites.

## Status boundary

This is a host integration build and a kernel foundation, not a bootable operating system. A bootable image still needs a named CPU target, boot protocol, architecture entry path, linker layout, early console, exception/interrupt vectors, page management, device drivers and QEMU or hardware validation.

See docs/ECOSYSTEM_INTEGRATION.md and ecosystem/manifest.json.
