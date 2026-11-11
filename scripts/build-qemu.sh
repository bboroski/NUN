#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${BUILD_DIR:-$ROOT/build/qemu}"
mkdir -p "$BUILD/iso/boot/grub"
CC="${CC:-gcc}"
AS="${AS:-as}"
LD="${LD:-ld}"
# Early bring-up does not enable CR0/CR4 FPU/SIMD state. Keep compiler output
# in general-purpose registers until the kernel owns FPU initialization/context switching.
CFLAGS="-std=gnu11 -O2 -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie -fno-asynchronous-unwind-tables -m64 -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -mno-80387 -msoft-float -mcmodel=small"
"$CC" $CFLAGS -c "$ROOT/kernel/bringup.c" -o "$BUILD/bringup.o"
"$AS" --64 "$ROOT/boot/boot.S" -o "$BUILD/boot.o"
"$LD" -m elf_x86_64 -nostdlib -z max-page-size=0x1000 -T "$ROOT/boot/linker.ld" "$BUILD/boot.o" "$BUILD/bringup.o" -o "$BUILD/kernel.elf"

# Guard the early boot path against accidental compiler-generated SIMD use.
objdump -d -Mintel "$BUILD/kernel.elf" > "$BUILD/kernel.dis"
if grep -Eiw '\\b(xmm[0-9]+|ymm[0-9]+|zmm[0-9]+|mm[0-7])\\b' "$BUILD/kernel.dis"; then
  echo "error: early NUN kernel contains SIMD instructions before FPU state setup" >&2
  exit 1
fi

if command -v grub-file >/dev/null 2>&1; then
  grub-file --is-x86-multiboot2 "$BUILD/kernel.elf"
fi
cp "$BUILD/kernel.elf" "$BUILD/iso/boot/kernel.elf"
cp "$ROOT/boot/grub.cfg" "$BUILD/iso/boot/grub/grub.cfg"
grub-mkrescue -o "$BUILD/nun-qemu.iso" "$BUILD/iso" >/dev/null
printf 'NUN QEMU image: %s\n' "$BUILD/nun-qemu.iso"
