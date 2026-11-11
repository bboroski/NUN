#!/usr/bin/env bash
set -Eeuo pipefail

ROOT="$(cd -- "$(dirname -- "$0")/.." && pwd -P)"
BUILD_DIR="$ROOT/build/e2k"
OUT="$BUILD_DIR/NUN-E2K-host"
CC="${E2K_CC:-lcc}"
PYTHON="${PYTHON:-python3}"

command -v "$CC" >/dev/null 2>&1 || {
    echo "MCST E2K LCC compiler not found: $CC" >&2
    echo "Install the vendor SDK or set E2K_CC to its LCC executable." >&2
    exit 2
}
command -v "$PYTHON" >/dev/null 2>&1 || {
    echo "Python 3 is required to write the E2K ELF manifest." >&2
    exit 2
}

mkdir -p -- "$BUILD_DIR"
read -r -a EXTRA_CFLAGS <<< "${E2K_CFLAGS:-}"

echo "[NUN] Building native E2K Linux runtime with $CC"
"$CC" -std=c11 -O2 -Wall -Wextra -Werror \
    -D_POSIX_C_SOURCE=200809L \
    "${EXTRA_CFLAGS[@]}" \
    "$ROOT/platforms/e2k/nun_host.c" -o "$OUT"

"$PYTHON" "$ROOT/scripts/write_e2k_manifest.py" \
    --binary "$OUT" \
    --output "$BUILD_DIR/NUN-E2K-host.json" \
    --compiler "LCC" \
    --compiler-version "$("$CC" --version 2>&1 | head -n 1 || true)"

echo "[NUN] E2K runtime: $OUT"
echo "[NUN] E2K manifest: $BUILD_DIR/NUN-E2K-host.json"
echo "[NUN] Compile target is E2K Linux userspace, not a standalone bootloader/kernel."
