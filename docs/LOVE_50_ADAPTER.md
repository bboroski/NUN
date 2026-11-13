# LOVE-50 adapter — NUN

**Repository:** `bboroski/NUN`  
**Role:** Minimal x86_64 kernel substrate and boot/runtime contracts  
**Status:** SPECIFIED — mapping only; runtime conformance is not implied.  
**Canonical doctrine:** [Klipot LOVE-50](https://github.com/bboroski/Klipot/blob/feat/love-50-chinese-proverbs-20261113/docs/LOVE_50_CHINESE_DOCTRINE.md)

## Why this profile exists

The kernel's application of care is strict containment: validate handoff data, preserve memory boundaries, provide diagnostic evidence and refuse unsafe states. No new kernel authority is implied by LOVE-50.

## Repository-specific application

### Fate / compatibility
Version boot-information contracts and validate architecture, image format, memory map and capability assumptions before use.

### Fidelity / trust
Keep boot stages explicit and record image/manifest provenance. A valid header is not proof of a bootable, authenticated or trusted image.

### Responsiveness / care
Use bounded diagnostics and deterministic fault paths. Do not continue after invalid memory ranges or impossible state transitions.

### Harmony / integration
NUN and NUNLauncher communicate only through versioned image and boot-information contracts; qualify Limine and Multiboot2 tracks separately.

### Wisdom / restraint
Host tests or QEMU runs prove only the tested path. Do not imply unsupported devices, architectures, a complete OS or production readiness.

## Local verification

- `make test`
- `bash scripts/build-qemu.sh`
- `bash scripts/run-qemu.sh`
- `Review NUN QEMU kernel bring-up workflow and BIOS/UEFI serial evidence.`
- `For boot/memory changes, test malformed maps, boundary overflow and invalid handoff metadata.`

## Non-goals and safety boundaries

- No kernel feature is added solely because a proverb suggests togetherness.
- No physical hardware support inferred from emulator evidence.
- No boot continuation after a failed safety gate.

## Adoption state

This document establishes an interpretation only. A control is IMPLEMENTED only when code/configuration exists, and VERIFIED only when repository-native checks and target-specific evidence pass. Keep unknown, planned, partial and failed states distinct. Existing security, privacy, ABI, platform, licensing and release requirements remain authoritative.

---
*The Chinese expressions are engineering prompts, not claims that software has human emotions.*
