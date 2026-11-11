# NUN
**Minimal Kernel Substrate**

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md)

An early x86_64 kernel bring-up and specification project. It is not a complete operating system.

## Features
- Versioned boot handoff contract with NUNLauncher.
- Architecture layer for CPU entry, privilege transitions, exceptions and interrupt vectors.
- Memory-model, early-allocation, virtual-addressing and protection requirements.
- Separate Multiboot2 and Limine boot targets with QEMU checks.

## Run and test
```bash
bash scripts/build-qemu.sh
bash scripts/run-qemu.sh
# Limine target
make iso
make test
```

## Scope
Multiboot2 and Limine targets are separate. QEMU smoke tests do not qualify physical hardware. E2K userspace support is not a bare-metal E2K kernel.

## Project documentation
Visual proof for a kernel belongs in boot logs and QEMU CI results; no GUI screenshot is claimed.

## Related
[Boot contract](docs/) · [NUNLauncher](https://github.com/bboroski/NUNLauncher) · [Fragile-Baby](https://github.com/bboroski/Fragile-Baby)

## License
No root license file is declared; review the repository terms before reuse.

---
**海枯石烂心不变。**
*Even if seas dry up and rocks crumble, the heart will not change.*
— Traditional Chinese expression
