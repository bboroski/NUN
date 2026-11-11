#!/usr/bin/env bash
set -Eeuo pipefail

ROOT="$(cd -- "$(dirname -- "$0")/.." && pwd -P)"
BUILD_DIR="$ROOT/build"
if [[ "$BUILD_DIR" == "/" || "$BUILD_DIR" == "$ROOT" ]]; then
    echo "Refusing unsafe BUILD_DIR: $BUILD_DIR" >&2
    exit 2
fi

LIMINE_VERSION="12.9.6"
LIMINE_SHA256="f00bf1e994e969905b3a8259e2414809174491a8e8ae87e7129095745dfc6857"
ARCHIVE="$BUILD_DIR/downloads/limine-$LIMINE_VERSION.tar.bz2"
SOURCE="$BUILD_DIR/limine-src"
STAGE="$BUILD_DIR/limine-stage"
ISO_ROOT="$BUILD_DIR/iso-root"
ISO="$BUILD_DIR/NUN.iso"
JOBS="$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '2')"

need() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "Missing build dependency: $1" >&2
        exit 2
    }
}

for command in curl sha256sum tar make gcc clang ld.lld objcopy objdump readelf nasm mcopy xorriso python3; do
    need "$command"
done

mkdir -p -- "$BUILD_DIR/downloads"
if [[ ! -f "$ARCHIVE" ]] ||
   ! printf '%s  %s\n' "$LIMINE_SHA256" "$ARCHIVE" | sha256sum --check --status; then
    echo "[NUN] Downloading pinned Limine $LIMINE_VERSION..."
    curl --fail --location --retry 3 \
        "https://github.com/Limine-Bootloader/Limine/releases/download/v$LIMINE_VERSION/limine-$LIMINE_VERSION.tar.bz2" \
        --output "$ARCHIVE"
fi
printf '%s  %s\n' "$LIMINE_SHA256" "$ARCHIVE" | sha256sum --check

rm -rf -- "$SOURCE" "$STAGE" "$ISO_ROOT"
mkdir -p -- "$SOURCE" "$STAGE" "$ISO_ROOT"
tar -xjf "$ARCHIVE" --strip-components=1 -C "$SOURCE"

echo "[NUN] Building Limine BIOS + x86_64 UEFI support..."
(
    cd -- "$SOURCE"
    CC_FOR_TARGET=clang LD_FOR_TARGET=ld.lld \
        OBJCOPY_FOR_TARGET=objcopy OBJDUMP_FOR_TARGET=objdump READELF_FOR_TARGET=readelf \
        ./configure \
        --enable-bios \
        --enable-bios-cd \
        --enable-uefi-x86-64 \
        --enable-uefi-cd \
        --prefix=/usr
    make -j"$JOBS"
    make install DESTDIR="$STAGE"
)

LIMINE_SHARE="$STAGE/usr/share/limine"
LIMINE_TOOL="$STAGE/usr/bin/limine"
for artifact in limine-bios.sys limine-bios-cd.bin limine-uefi-cd.bin BOOTX64.EFI; do
    [[ -s "$LIMINE_SHARE/$artifact" ]] || {
        echo "Limine build did not produce $artifact" >&2
        exit 1
    }
done
[[ -x "$LIMINE_TOOL" ]] || {
    echo "Limine host utility is missing: $LIMINE_TOOL" >&2
    exit 1
}

echo "[NUN] Compiling freestanding x86_64 kernel..."
mkdir -p -- "$BUILD_DIR/kernel"
gcc \
    -std=gnu11 -O2 -g -Wall -Wextra -Werror \
    -ffreestanding -fPIE -fno-builtin -fno-common \
    -fno-stack-protector -fno-stack-check -fno-lto \
    -fno-asynchronous-unwind-tables -fno-unwind-tables \
    -m64 -march=x86-64 -mgeneral-regs-only -mno-red-zone \
    -I"$ROOT/include" \
    -c "$ROOT/kernel/main.c" -o "$BUILD_DIR/kernel/main.o"

ld.lld -m elf_x86_64 -nostdlib \
    -zmax-page-size=0x1000 -pie -ztext \
    -T "$ROOT/kernel/linker.ld" --build-id=none \
    "$BUILD_DIR/kernel/main.o" \
    -o "$BUILD_DIR/kernel.elf"

readelf -h "$BUILD_DIR/kernel.elf" | grep -q 'ELF64' || {
    echo "Kernel is not an ELF64 image" >&2
    exit 1
}
readelf -h "$BUILD_DIR/kernel.elf" | grep -Eq 'Type:.*DYN' || {
    echo "Kernel must be a position-independent ELF image" >&2
    exit 1
}
if readelf -l "$BUILD_DIR/kernel.elf" | grep -q 'INTERP'; then
    echo "Freestanding kernel must not require a dynamic interpreter" >&2
    exit 1
fi
for symbol in base_revision requests_start requests_end framebuffer_request memory_map_request bootloader_info_request; do
    readelf -sW "$BUILD_DIR/kernel.elf" | grep -Fq "$symbol" || {
        echo "Limine request symbol was not retained: $symbol" >&2
        exit 1
    }
done

echo "[NUN] Assembling hybrid BIOS/UEFI boot image..."
mkdir -p -- "$ISO_ROOT/boot/NUN" "$ISO_ROOT/boot/limine" "$ISO_ROOT/EFI/BOOT"
install -m 0644 "$BUILD_DIR/kernel.elf" "$ISO_ROOT/boot/NUN/kernel.elf"
install -m 0644 "$ROOT/boot/limine.conf" "$ISO_ROOT/boot/limine/limine.conf"
install -m 0644 "$LIMINE_SHARE/limine-bios.sys" "$ISO_ROOT/boot/limine/limine-bios.sys"
install -m 0644 "$SOURCE/COPYING" "$ISO_ROOT/boot/limine/COPYING"
install -m 0644 "$LIMINE_SHARE/limine-bios-cd.bin" "$ISO_ROOT/boot/limine/limine-bios-cd.bin"
install -m 0644 "$LIMINE_SHARE/limine-uefi-cd.bin" "$ISO_ROOT/boot/limine/limine-uefi-cd.bin"
install -m 0644 "$LIMINE_SHARE/BOOTX64.EFI" "$ISO_ROOT/EFI/BOOT/BOOTX64.EFI"

xorriso -as mkisofs \
    -R -r -J \
    -b boot/limine/limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot boot/limine/limine-uefi-cd.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    -o "$ISO" "$ISO_ROOT"

"$LIMINE_TOOL" bios-install "$ISO"

echo
echo "[NUN] Boot image ready: $ISO"
echo "[NUN] SHA-256: $(sha256sum "$ISO" | awk '{print $1}')"
python3 "$ROOT/scripts/write_image_manifest.py" \
    --image "$ISO" \
    --kernel "$BUILD_DIR/kernel.elf" \
    --output "$BUILD_DIR/NUN.image.json" \
    --limine-version "$LIMINE_VERSION" \
    --limine-sha256 "$LIMINE_SHA256"
echo "[NUN] Image manifest: $BUILD_DIR/NUN.image.json"
echo "[NUN] BIOS and x86_64 UEFI media are included."
echo "[NUN] This is a bootable kernel prototype, not a complete desktop OS."
