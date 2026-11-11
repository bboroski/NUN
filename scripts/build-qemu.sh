#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${BUILD_DIR:-$ROOT/build/qemu}"
mkdir -p "$BUILD/iso/boot/grub"
CC="${CC:-gcc}"
AS="${AS:-as}"
LD="${LD:-ld}"
CFLAGS="-std=gnu11 -O2 -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie -fno-asynchronous-unwind-tables -m64 -mno-red-zone -mcmodel=small"
"$CC" $CFLAGS -c "$ROOT/kernel/bringup.c" -o "$BUILD/bringup.o"
"$AS" --64 "$ROOT/boot/boot.S" -o "$BUILD/boot.o"
"$LD" -m elf_x86_64 -nostdlib -z max-page-size=0x1000 -T "$ROOT/boot/linker.ld" "$BUILD/boot.o" "$BUILD/bringup.o" -o "$BUILD/kernel.elf"
if command -v grub-file >/dev/null 2>&1; then
  grub-file --is-x86-multiboot2 "$BUILD/kernel.elf"
fi
cp "$BUILD/kernel.elf" "$BUILD/iso/boot/kernel.elf"
cp "$ROOT/boot/grub.cfg" "$BUILD/iso/boot/grub/grub.cfg"
grub-mkrescue -o "$BUILD/nun-qemu.iso" "$BUILD/iso" >/dev/null
printf 'NUN QEMU image: %s\n' "$BUILD/nun-qemu.iso"
