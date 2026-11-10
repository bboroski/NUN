# NUN ecosystem integration map

NUN composes the existing engineering repositories through adapters and explicit interfaces. The repositories remain independently buildable and independently governed. This map records intended ownership; it is not proof that the modules are linked into a running OS image.

## Repository roles

| Repository | Existing project identity | NUN responsibility | Integration state |
|---|---|---|---|
| [Kloz](https://github.com/bboroski/Kloz) | PBSC — Portable Bare-Metal Substrate Core | fixed-width status ABI, caller-owned arenas and versioned platform operations | contract source; NUN adapter planned |
| [CO](https://github.com/bboroski/CO) | DLO — Deterministic Lifecycle Orchestrator | bounded service lifecycle, staged initialization and cleanup | lifecycle model reusable; kernel supervisor adapter planned |
| [SaintSI](https://github.com/bboroski/SaintSI) | RIAE — Runtime Integrity Assurance Engine | progress observations, timeout state and accidental-corruption checks | optional observer adapter planned |
| [ReaperCripple](https://github.com/bboroski/ReaperCripple) | EFMF — Embedded Firmware Manufacturing Fabric | board profiles, BSP bring-up, calibration, fixtures and target evidence | target-specific adapter planned |
| [SAMBU](https://github.com/bboroski/SAMBU) | SMAE — Scientific Modeling & Automation Environment | language tooling, IDE, workflow models, architecture metadata and project automation | tooling integration planned |
| [ToolNG](https://github.com/bboroski/ToolNG) | WDEP — Windows Device Evidence Probe | host-side device diagnostics and evidence import during development | external Windows adapter only |
| [Klipot](https://github.com/bboroski/Klipot) | EICP — Ecosystem Integration & Control Plane | module discovery, dependency views, docs and operator-facing registry | external control plane; never a kernel dependency |
| [NUNLauncher](https://github.com/bboroski/NUNLauncher) | NUN Android Home Shell | Android Home experience and future NUN service client | separate application; Android bridge planned |

## Dependency direction

~~~text
                 SAMBU toolchain / IDE
                          |
                          v
NUNLauncher ----> NUN user services <---- Klipot (metadata only)
                          |
                   service manager
                    /    |     \
                   v     v      v
                 DLO   RIAE   language services
                   \     |      /
                    \    |     /
                     PBSC ABI
                         |
                   BSP / EFMF
                         |
                     target board

ToolNG remains an optional Windows-side diagnostic source.
~~~

The diagram shows intended composition. It does not imply that Klipot or ToolNG is linked into the kernel, that SAMBU's Python runtime is embedded, or that optional native adapters are already implemented.

## Contract rules

1. **Ownership:** each repository remains authoritative for its own code and releases.
2. **No cyclic dependencies:** NUN adapters depend on published contracts; base libraries do not import NUN.
3. **Versioning:** every ABI and manifest has a major version and explicit compatibility rules.
4. **Memory:** buffers have caller-visible capacity, alignment and ownership; no hidden heap in early boot.
5. **Capabilities:** a manifest may request a permission but cannot grant it. The runtime broker decides.
6. **Time:** tick frequency, unit, width and rollover limits are explicit at every adapter boundary.
7. **Evidence:** distinguish implemented, adapter-ready, planned, simulated, unknown and not tested.
8. **Security:** CRC-32 is not cryptographic authenticity; hardware reset, watchdog and device write actions require explicit target policy.
9. **Distribution:** Klipot is metadata/control-plane UI; it is not a privileged runtime dependency.
10. **Android:** NUNLauncher uses Android APIs while running on Android. It does not replace Android's kernel or become NUN merely by setting itself as the HOME activity.

## Current scope

NUN contains an initial C11 ABI header and host smoke test. Every other integration described above remains a design contract or roadmap item until a concrete adapter, build target and test are committed in its owning repository.
