#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/build"
VERSION="12.9.6"
ARCHIVE="$BUILD/limine-binary.tar.gz"
EXPECTED="b38aabb042b0c0f9f562b7741be3c1204838d7bbacfa78bb411c674bd5b984ee"
URL="https://github.com/Limine-Bootloader/Limine/releases/download/v$VERSION/limine-binary.tar.gz"

for tool in curl tar sha256sum xorriso find install; do
  command -v "$tool" >/dev/null || { echo "Missing tool: $tool" >&2; exit 1; }
done
mkdir -p "$BUILD/limine-dist"
curl --fail --location --silent --show-error "$URL" -o "$ARCHIVE"
echo "$EXPECTED  $ARCHIVE" | sha256sum --check --status || {
  echo "Limine v$VERSION archive checksum mismatch" >&2
  exit 1
}
tar -xzf "$ARCHIVE" -C "$BUILD/limine-dist"

find_one() {
  find "$BUILD/limine-dist" -type f -name "$1" -print -quit
}
BIOS_SYS="$(find_one limine-bios.sys)"
BIOS_CD="$(find_one limine-bios-cd.bin)"
UEFI_CD="$(find_one limine-uefi-cd.bin)"
EFI_APP="$(find_one BOOTX64.EFI)"
HOST_TOOL="$(find_one limine)"

for item in "$BIOS_SYS" "$BIOS_CD" "$UEFI_CD" "$EFI_APP" "$HOST_TOOL"; do
  [[ -n "$item" && -f "$item" ]] || { echo "Incomplete Limine binary package" >&2; exit 1; }
done

ISO_ROOT="$BUILD/iso-root"
rm -rf "$ISO_ROOT"
mkdir -p "$ISO_ROOT/boot" "$ISO_ROOT/EFI/BOOT"
install -m 0644 "$BUILD/NUN.elf" "$ISO_ROOT/boot/NUN.elf"
install -m 0644 "$ROOT/limine.conf" "$ISO_ROOT/limine.conf"
install -m 0644 "$BIOS_SYS" "$ISO_ROOT/limine-bios.sys"
install -m 0644 "$BIOS_CD" "$ISO_ROOT/limine-bios-cd.bin"
install -m 0644 "$UEFI_CD" "$ISO_ROOT/limine-uefi-cd.bin"
install -m 0644 "$EFI_APP" "$ISO_ROOT/EFI/BOOT/BOOTX64.EFI"

xorriso -as mkisofs -R -r -J -b limine-bios-cd.bin \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image \
  --protective-msdos-label "$ISO_ROOT" -o "$BUILD/NUN.iso"

"$HOST_TOOL" bios-install "$BUILD/NUN.iso"
echo "Created $BUILD/NUN.iso"
