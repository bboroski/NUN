# NUN boot and build guide

## Toolchain

Use x86-64 GCC or Clang, GNU Make and binutils. curl fetches pinned build inputs; xorriso creates a hybrid ISO; QEMU runs the smoke test.

First target: QEMU x86-64 with a Limine BIOS/UEFI hybrid ISO. A successful emulator boot is not hardware qualification.

## Reproducible inputs

The Limine protocol header is fetched from a pinned upstream commit and checked against its Git blob ID. The Limine binary release is pinned to v12.9.6 and its published SHA-256 digest is verified before unpacking. Release builds must not use an unpinned latest branch.

## Commands

~~~bash
make deps       # fetch and verify the boot protocol header
make ecosystem-sources # initialize all pinned repository submodules
make check      # host-side allocator tests
make kernel     # link build/NUN.elf
make iso        # create build/NUN.iso with BIOS and UEFI paths
make run        # start QEMU with serial diagnostics
make clean
~~~

Cross compiler:

~~~bash
make kernel CROSS_COMPILE=x86_64-elf-
~~~

## Boot configuration

Limine loads boot/NUN.elf using its native protocol. The kernel requests the memory map, HHDM offset, framebuffer and bootloader metadata. Every required response is checked before use.

## Expected first-boot output

The COM1 serial log reports kernel entry, protocol validation, bootloader name/version when returned, memory-map count and usable bytes, first page allocation, and a boot readiness marker.

The framebuffer currently displays only the black/yellow visual marker. It is not a full desktop or text console.

## Debugging

~~~bash
qemu-system-x86_64 -M q35 -m 512M -cdrom build/NUN.iso -serial stdio -no-reboot -no-shutdown
~~~

If Limine does not provide a requested structure, the kernel halts and records the missing response rather than dereferencing a null pointer.

## Current limitations

There is no scheduler, user process, VFS, storage/PCI driver, network stack, secure-boot enrollment, update manager or recovery environment yet. The initial page allocator is monotonic: it does not free or reclaim pages. Do not install this image on a production device.
