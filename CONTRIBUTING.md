# Contributing to NUN

NUN is an operating-system/kernel project. Changes to the boot chain, kernel, modules, linker configuration, and platform support require explicit architecture and compatibility notes. Host tests and emulation do not prove physical hardware support.

## Required workflow

1. Open or link an Issue and identify the boot stage, kernel subsystem, target architecture, and affected modules.
2. Use a focused branch: `feat/<topic>`, `fix/<topic>`, `docs/<topic>`, or `security/<topic>`.
3. Preserve boot/image manifest schemas and document format changes.
4. Keep privileged operations explicit and avoid embedding secrets or device-specific credentials.
5. Never commit generated boot images, private firmware, or local build trees unless a documented fixture requires them.

## Required validation

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
python one.py
```

Run the QEMU boot/smoke workflow when changing boot, linker, or image behavior. Physical board support requires separate hardware evidence.

## Commits and releases

Use Conventional Commit subjects (`feat:`, `fix:`, `docs:`, `refactor:`, `perf:`, `test:`, `build:`, `ci:`, `chore:`). Breaking changes require `!:` and a `BREAKING CHANGE:` footer. Releases use SemVer; update boot-image, ABI, and migration notes before tagging.

## Pull request checklist

- Identify architecture, boot stage, and manifest impact.
- Include build, host-test, and emulation results.
- Document boot recovery and rollback for boot-chain changes.
- Do not state hardware qualification without board-level testing.
