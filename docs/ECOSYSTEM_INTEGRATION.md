# NUN ecosystem integration

NUN is the integration root for the bboroski OS ecosystem. Source repositories retain their own histories, APIs, release cycles and notices. NUN pins selected upstream commits as Git submodules and integrates them through explicit adapters; it does not flatten every project into the kernel.

## Pinned repository layout

| Repository | Pinned path | Integration boundary | Execution class |
|---|---|---|---|
| SAMBU | `modules/SAMBU` | Language, compiler, SDK and developer tools | Host tooling |
| ToolNG | `modules/ToolNG` | Native policy/evidence library | Host-testable library; not kernel authorization |
| ReaperCripple | `modules/ReaperCripple` | BSP, bootstrap and recovery profiles | Host-tested library; board adapters remain explicit |
| Kloz | `modules/Kloz` | Versioned platform ABI and caller-owned resources | Host-tested C library; freestanding audit still required |
| CO | `modules/CO` | Deterministic component lifecycle | Host-tested C library; not a scheduler |
| SaintSI | `modules/SaintSI` | Heartbeats, health observations and CRC | Host-tested C library; observer-only |
| Klipot | `modules/Klipot` | Integration planning and portal | Host tooling |
| NUNLauncher | `modules/NUNLauncher` | Versioned image header/range admission contract | Host C11 library; no firmware handoff |
| Luci | `modules/Luci` | Read-only registry, manifest and policy checks | Host Python tooling |
| Fragile-Baby | `modules/Fragile-Baby` | E2K evidence matrix and research | Quarantined host research tooling |
| Hellduck | `federation/Hellduck` | Federation registry and contract consistency | Host metadata validation only |

The exact immutable commit pins for the four newly connected repositories are recorded in [nun-ecosystem-pins.json](../ecosystem/nun-ecosystem-pins.json). The gitlinks in the tree are the authoritative checkout pins.

## Implemented NUNLauncher interface

NUNLauncher exposes a portable C11 image-header validator. NUN now builds it as a separate host target and adds `NUN.BootAdmission`, a narrow adapter that translates NUN architecture and memory-region enums into NUNLauncher's vocabulary.

The adapter:
- maps x86_64, AArch64, RISC-V64 and E2K explicitly (enum numeric values are not assumed to match);
- maps available RAM to usable RAM, reserved/kernel ranges to reserved, and MMIO to MMIO;
- rejects unknown region types, nonzero flags, invalid counts and unsupported architectures;
- delegates header, bounds, alignment, CRC and entry validation to NUNLauncher.

This is pre-load metadata/range validation. It does not authenticate the publisher, load or map the image, replace GRUB/Limine/firmware, perform a handoff, or execute code. It is not linked into the freestanding kernel. NUN's existing boot-info validator remains a separate post-load handoff contract.

## Host control-plane integrations

- **Luci:** `one.py check` runs Luci's own standard-library unit tests and read-only registry/manifest validation.
- **Fragile-Baby:** the E2K evidence-advisor tests run as a separate host test suite. Evidence coverage is not a hardware qualification claim.
- **Hellduck:** `ecosystem/validate.py` validates the pinned federation registry and relationship contracts offline. Hellduck does not become a boot-time or runtime dependency.

All three are host-only. None is imported into kernel CMake targets, kernel address space, or the boot image.

## Provenance and license gate

The four repositories are linked by immutable gitlinks; their source is not copied into NUN or relicensed. The current root of each newly connected repository has no `LICENSE` file. Preserve each repository's notices and source history, and complete a license/provenance review before redistributing built binaries or shipping their contents in an OS image. Submodule presence is not a blanket license grant.

## Existing native runtime path

The existing NUN host build composes Kloz, CO, SaintSI, ReaperCripple and ToolNG. All memory storage remains caller-owned; lifecycle plans are validated before callbacks; rollback is reverse ordered; health observation does not actuate hardware; policy evidence does not grant kernel privileges. No implicit thread creation, MMIO, network upload, watchdog activation or device mutation is permitted.

## Verification

Run from a recursive clone:

```sh
git submodule update --init --recursive
python one.py
```

The one-knob build compiles NUNLauncher and the NUN adapter, runs CTest, then runs Luci, Fragile-Baby and Hellduck host checks. Optional pytest suites for SAMBU and Klipot run when pytest is installed.

A passing host build is not proof of a bootable OS, firmware handoff, Secure Boot, signed publisher identity, E2K target support, graphics, networking, transactional updates or physical-hardware qualification.
