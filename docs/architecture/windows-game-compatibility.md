# NUN Windows Game Compatibility

**Status:** architecture specification  
**Implementation status:** not implemented  
**Target:** user-mode Windows games and game launchers on a future NUN installation  
**Canonical repository:** NUN

## 1. Goal and honest boundary

NUN is currently a specification scaffold. It has no kernel source, process manager, executable loader, GPU driver, system-call implementation, build pipeline, or bootable image. This document therefore defines an implementation contract; it does not claim that NUN can currently execute an `.exe`.

The goal is to let a user install or select a Windows game, inspect its requirements, create an isolated compatibility profile, and launch it from NUN without installing Windows as the primary operating system.

A Windows executable is a user-mode program, not a kernel image. NUNLauncher must continue to validate and hand off the NUN kernel image; it must not treat a game `.exe` as a boot target.

The design follows a Wine/Proton-style compatibility model: implement the application-facing Windows interfaces and translate them to NUN services. Wine is an API compatibility layer rather than a CPU emulator. Proton builds on Wine and additional components for Steam Play. These projects are architectural references, not dependencies that NUN already contains.

## 2. Required execution model

The end-to-end path is:

```text
NUN Game Center
  -> Game Profile Resolver
  -> PE/COFF Inspector and User-Mode Loader
  -> Win32 / NT Compatibility Runtime
  -> Windows DLL and system-service shims
  -> Graphics, audio, input, storage and network adapters
  -> NUN process, memory, scheduler, IPC and device services
```

The compatibility runtime must run in user mode. It must not require a game to execute with kernel privilege. A game receives narrowly scoped handles to files, devices, network endpoints, shared memory and synchronization objects.

### 2.1 Game Center and profile resolver

The control plane must:

- discover executable candidates without executing them;
- let the user select the real game entry point rather than a helper/updater executable;
- inspect PE architecture, subsystem, imports, dependencies and graphics API hints;
- create a per-game prefix containing virtual drive mappings, registry-like state, DLL overrides, environment and saved settings;
- select a compatibility runtime version and graphics translation backend;
- check required NUN capabilities before launch;
- show an explicit compatibility result and missing prerequisites;
- launch, stop and monitor the game process tree;
- preserve logs, crash records and a reproducible profile snapshot.

Profiles are data, not executable policy. Importing a profile must never run the referenced executable or grant permissions.

### 2.2 PE/COFF inspection and loader

The first executable format target is Windows PE32 (x86) and PE32+ (x86-64). ARM64 PE is a later target and is not implied by schema support.

The inspector must validate, before mapping:

- DOS header and PE signature;
- COFF machine type and optional-header magic;
- section table, alignment, image size and entry-point bounds;
- data-directory bounds and RVA-to-file-offset conversions;
- import and delay-import tables;
- relocation directory and relocation types;
- TLS directory and callback bounds;
- exception/unwind metadata for x86-64;
- subsystem and DLL characteristics;
- certificate-table bounds and any configured trust policy.

The loader must map sections with correct permissions, apply relocations, resolve imports against the selected compatibility environment, establish thread-local storage, create the initial process/thread context and transfer control to the validated entry point.

Malformed, unsupported, ambiguous or out-of-range structures must fail closed. A PE certificate is provenance data, not proof that the executable is safe. User consent, digest recording, trust policy and process isolation remain required.

Do not execute a binary during discovery, validation, profile creation or metadata extraction. Do not load a game DLL into the kernel.

### 2.3 Win32 and NT compatibility runtime

A PE loader alone is not sufficient. Most games expect a large set of Windows user-mode APIs and observable process semantics.

The runtime must define versioned NUN interfaces for:

- process/thread creation, exit codes, handles and process-tree supervision;
- virtual memory reservation, commitment, protection and mapping;
- synchronization primitives, wait semantics, events, mutexes and semaphores;
- timers, clocks, sleep, waitable timers and asynchronous completion;
- file handles, sharing modes, file locking, directory enumeration and path normalization;
- registry-like per-prefix configuration and environment variables;
- DLL loading, search order, exports, imports and delay loading;
- exceptions, structured exception handling, x86-64 unwind and debugger hooks;
- window creation, messages, keyboard focus, DPI and display modes;
- sockets, DNS, TLS integration and network policy.

The initial API surface should prioritize the modules most common to desktop games, then expand from observed import traces and compatibility tests. Candidate families include NTDLL, KERNEL32/KERNELBASE, USER32, GDI32, ADVAPI32, WS2_32, WINMM, XINPUT and XAudio2. This list is a work plan, not a claim that those APIs are implemented.

NUN must not claim Windows 10/11 compatibility based on a version string alone. Each advertised compatibility target requires API behavior tests and per-game evidence.

### 2.4 Graphics pipeline

Graphics translation is a separate subsystem from PE loading.

The preferred long-term route is a NUN Vulkan implementation with translation adapters:

| Windows graphics interface | Planned adapter | Notes |
|---|---|---|
| Direct3D 8 | Dedicated D3D8 adapter or validated compatibility path | Legacy milestone; not covered by the initial baseline |
| Direct3D 9 / 10 / 11 | DXVK-style translation to Vulkan | Requires a conformant NUN Vulkan driver and synchronization support |
| Direct3D 12 | vkd3d-proton-style translation to Vulkan | Later milestone; requires descriptor, memory and feature coverage |
| OpenGL | Native OpenGL implementation or a documented translation path | Depends on the selected NUN graphics stack |
| Vulkan | NUN Vulkan loader and driver | Direct path when the game and driver agree on supported versions/extensions |

The Vulkan ecosystem identifies DXVK as a Direct3D 9/10/11-to-Vulkan layer and VKD3D-Proton as the Direct3D 12-on-Vulkan effort used by Proton. NUN must verify exact upstream versions, licenses, architecture dependencies and porting requirements before adopting any code.

The graphics capability gate must compare the game's requested API/features with the actual adapter and driver capabilities. Do not advertise DX12, ray tracing, shader model or feature-level support from the presence of a Vulkan loader alone.

### 2.5 Audio, input and peripherals

Provide independent adapters and test suites for:

- XAudio2 and legacy DirectSound / WinMM playback;
- WASAPI-style endpoint enumeration, shared/exclusive modes, sample-rate conversion and latency;
- XInput gamepads, DirectInput, Raw Input, keyboard, mouse, HID and hot-plug;
- rumble/haptics, controller remapping and device focus;
- audio device loss, suspend/resume and output switching.

Each adapter must declare supported interfaces and versions. Missing audio or input services must produce a clear capability error rather than a silent no-op.

### 2.6 Storage, registry and runtime dependencies

Each game profile receives a private compatibility prefix. The prefix must contain only the files and configuration needed by that game. It may expose explicit user-selected mappings to game libraries, removable media and save-data locations.

The runtime must implement Windows path rules, drive-letter mapping, case-sensitivity policy, reserved names, file sharing/locking and expected file metadata behavior. It must not expose host system directories by default.

Redistributables and game-owned DLLs must be identified by publisher, source, version and digest. System DLL replacement is controlled by an explicit, reversible profile override. Do not fetch DLLs from untrusted mirrors or silently replace game files.

### 2.7 CPU architecture and translation

The first practical target should be x86-64 NUN on x86-64 hardware, with a separately validated x86 compatibility path for 32-bit games. This target is proposed, not selected or implemented by the current repository.

Running x86/x64 games on ARM64 requires a tested instruction translator plus compatible system interfaces; PE parsing alone does not provide CPU translation. ARM64 game support must remain a separate readiness gate.

## 3. Security and compatibility boundaries

- Games execute as untrusted user processes with least-privilege handles.
- A game profile cannot request kernel privileges or load arbitrary kernel drivers.
- Kernel-mode anti-cheat drivers are unsupported by the initial design. Do not bypass anti-cheat, DRM, identity checks or multiplayer protections.
- User-mode anti-cheat and DRM compatibility is title-specific and must be explicitly tested; unknown status is not equivalent to support.
- Network, microphone, camera, removable-media and controller access require scoped policy.
- Each game has resource limits, crash isolation, process-tree shutdown and a recoverable prefix.
- Downloads and imported components require provenance, license review and digest verification.
- Runtime overrides must be reversible and recorded in the profile.
- A failed game must not corrupt the kernel, system service registry or another game's prefix.

Compatibility labels:

| Label | Evidence required |
|---|---|
| Not tested | No execution evidence |
| Loads | Process and initial window start; no claim of playability |
| Menu | Menu is usable; gameplay is not yet validated |
| Playable | A defined gameplay test passes with known limitations documented |
| Verified | Repeated regression suite passes on a named NUN build, driver, GPU and runtime |
| Blocked | A known missing requirement prevents launch |

The label must include the tested NUN build, CPU, GPU/driver, runtime version, graphics adapter and test date.

## 4. Capability contracts

The compatibility stack must use NUN's existing Capability Composition & Assimilation rules. Each module is a separately identified capability with a versioned ABI, declared interfaces, permissions, artifact digest, dependencies and resource limits.

Suggested future capability identities:

- `nun.game.center` — library and launch control plane;
- `nun.windows.pe-loader` — PE inspection and user-mode image loading;
- `nun.windows.nt-runtime` — process-facing NT/Win32 compatibility;
- `nun.windows.graphics.dxvk` — Direct3D 8/9/10/11 translation adapter, after its exact scope is validated;
- `nun.windows.graphics.vkd3d` — Direct3D 12 translation adapter;
- `nun.windows.audio` — Windows audio API adapters;
- `nun.windows.input` — controller, keyboard, mouse and HID adapters;
- `nun.windows.profile` — isolated per-game configuration and state.

These names are proposed identifiers only. No corresponding executable artifacts or services exist yet.

## 5. Delivery stages and acceptance gates

### Stage 0 — architecture and ABI

- Select the initial CPU architecture, boot protocol and kernel ABI.
- Define process, virtual memory, file, synchronization, clock, IPC and device contracts.
- Freeze the game profile schema and capability identifiers.
- Add static profile and matrix validation.
- **Exit evidence:** schema validation, documented ABI and negative tests for malformed metadata.

### Stage 1 — executable inspection

- Implement a non-executing PE32/PE32+ parser in a host-testable user-space library.
- Add fuzzing and malformed-file corpus tests.
- Verify machine type, bounds, imports and unsupported features.
- **Current prototype:** `tools/inspect_windows_pe.py` reads PE32/PE32+ headers, machine type, subsystem, section table and imported DLL names without executing the file. It is a host-side diagnostic tool, not NUN runtime code.
- **Current unit tests:** `python -m unittest discover -s tests -v`.
- **Inspect a file:** `python tools/inspect_windows_pe.py path/to/game.exe --json`.
- **Exit evidence:** parser tests and fuzzing; no process launch is claimed.

### Stage 2 — user processes and PE loading

- Implement user address spaces, executable mappings, relocations, DLL loading, TLS and process lifecycle.
- Add a minimal test executable and deterministic process diagnostics.
- **Exit evidence:** known test binaries start and exit correctly in an emulator; memory protections and isolation tests pass.

### Stage 3 — base Win32/NT APIs

- Implement the high-value API subset and compatibility DLL loader.
- Add API conformance tests for file, memory, threading, synchronization, timers and window messages.
- **Exit evidence:** a documented test application uses the implemented APIs without host-side shortcuts.

### Stage 4 — graphics, audio and input

- Bring up Vulkan and the driver contract first.
- Port or implement the required Direct3D translation adapters after upstream license and dependency review.
- Add audio, controller and window/display adapters.
- **Exit evidence:** repeatable graphics, audio, input and device-loss tests; named game tests.

### Stage 5 — Game Center and title qualification

- Implement import/install, library scanning, profile editor, launch/stop, runtime selection, logs and rollback.
- Test real games from clean prefixes and publish per-title compatibility records.
- **Exit evidence:** reproducible per-game test reports on a named NUN build and hardware configuration.

A stage cannot be marked complete because its documentation or schema exists. Mark it complete only after code, tests and the stated evidence are present.

## 6. Diagnostics and recovery

Every launch attempt should produce a structured record with:

- game profile ID and executable digest;
- PE machine, subsystem and import summary;
- NUN build, CPU and GPU driver identity;
- compatibility runtime and adapter versions;
- selected API translations and missing capabilities;
- process exit status, crash class and relevant log references;
- elapsed startup time and resource-limit events.

A failed launch must preserve the previous profile and saved data. The user must be able to disable a DLL override, revert a runtime version and restore a known-good prefix without reinstalling NUN.

## 7. Verification checklist

- [ ] PE32 and PE32+ parser rejects malformed headers, invalid bounds and unsupported machine types.
- [ ] Image mapping enforces section permissions, relocation rules and DEP/ASLR policy.
- [ ] Import resolution, DLL search and delay-loading behavior are deterministic.
- [ ] TLS, x86-64 unwind, exceptions and thread-local state pass tests.
- [ ] Process handles and synchronization match documented semantics.
- [ ] File path, sharing, lock, registry and environment behavior pass compatibility tests.
- [ ] Windowing, DPI, input focus and display changes pass emulator tests.
- [ ] Vulkan feature negotiation is checked against the actual driver.
- [ ] DXVK and vkd3d-proton are version-pinned and license-reviewed before integration.
- [ ] Audio routing, controller hot-plug and suspend/resume pass tests.
- [ ] Kernel-mode anti-cheat drivers are rejected by policy.
- [ ] Per-game sandbox, resource limits, process-tree shutdown and prefix rollback pass tests.
- [ ] Each compatibility label links to a reproducible test report.

## 8. References and implementation research

- [Wine — project overview and API compatibility model](https://www.winehq.org/about/)
- [ValveSoftware/Proton — Steam Play compatibility tool based on Wine and additional components](https://github.com/ValveSoftware/Proton)
- [ValveSoftware/dxvk — Direct3D 9/10/11 translation to Vulkan](https://github.com/ValveSoftware/dxvk)
- [Vulkan — porting and Direct3D translation overview](https://www.vulkan.org/porting)
- [Steam Support — compatibility tool selection and troubleshooting](https://help.steampowered.com/en/faqs/view/7DD4-C618-182E-0E49)

Upstream projects are references and potential porting candidates. NUN must not claim to contain or support them until their sources, licenses, dependencies, build targets and tests are integrated.

## 9. Current state

This repository change adds architecture, machine-readable planning artifacts, and a host-side PE metadata inspector with synthetic unit tests. The inspector can report PE32/PE32+ architecture, subsystem, sections and imported DLL names; it does not load or execute the file. It is not part of a NUN boot image. No PE process loader, Windows API implementation, Vulkan driver, audio stack, game launcher executable or bootable operating system exists yet. Therefore **NUN still cannot launch Windows games today**. Actual gameplay comes only after the process, API, driver and compatibility stages above are implemented.
