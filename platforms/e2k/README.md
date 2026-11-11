# E2K / Elbrus platform profile

This profile enables 64-bit little-endian E2K Linux user-space builds with MCST LCC. ELF machine: `EM_MCST_ELBRUS = 175`; Linux `uname -m = e2k`.

Native builds use Elbrus Linux, LCC and matching runtime libraries. Cross builds use x86_64 Linux, the MCST LCC cross system and target sysroot. The shared CMake toolchain is `cmake/toolchains/e2k-linux-lcc.cmake`; set `E2K_LCC` or `E2K_CC` and optional `E2K_SYSROOT`.

The supported deployment route is a native Linux process: MCST PNS initializes the physical board and starts Elbrus Linux, then the project runs in userspace. `qemu-e2k` is user-mode Linux application emulation, not a full system/firmware emulator. Generic Limine does not list E2K as supported. An x86 boot image is not an Elbrus boot image.

Elbrus Linux families include v2-v7. Vendor SDK targets are model-specific. The published QEMU user-emulator manual lists v2-v6 and does not qualify v7 emulation. Cross-compile success is not runtime proof; qualify on a native E2K runner or hardware.

References: [MCST downloads](https://dev.mcst.ru/download/), [SDK](https://www.mcst.ru/SDK), [Elbrus Linux](https://www.mcst.ru/elbrus_linux), [PNS](https://www.mcst.ru/boot), [qemu-e2k](https://www.mcst.ru/2025-12-25_qemu-e2k), [Limine architectures](https://github.com/Limine-Bootloader/Limine).
