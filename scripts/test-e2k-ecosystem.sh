#!/usr/bin/env bash
set -Eeuo pipefail

ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd -P)"
BUILD_DIR="$ROOT/build/e2k-ecosystem"
CC="${E2K_CC:-lcc}"
JOBS="${NUN_E2K_BUILD_JOBS:-2}"

if [[ "$(uname -m)" != "e2k" ]]; then
    echo "E2K ecosystem qualification requires a native E2K Linux runner (uname -m=e2k)." >&2
    echo "Cross-builds and user-mode emulation are intentionally not accepted as hardware proof." >&2
    exit 3
fi

command -v "$CC" >/dev/null 2>&1 || {
    echo "MCST E2K LCC compiler not found: $CC" >&2
    exit 2
}
command -v cmake >/dev/null 2>&1 || {
    echo "CMake is required for the integrated NUN build." >&2
    exit 2
}
command -v ctest >/dev/null 2>&1 || {
    echo "CTest is required for the integrated NUN test suite." >&2
    exit 2
}

echo "[NUN] Native E2K ecosystem qualification"
echo "[NUN] Host: $(uname -a)"
echo "[NUN] Compiler: $CC"
"$CC" --version | head -n 1 || true

E2K_CC="$CC" E2K_LCC="$CC" cmake -S "$ROOT" -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$ROOT/cmake/toolchains/e2k-linux-lcc.cmake" \
    -DE2K_LCC="$CC" \
    -DNUN_BUILD_TESTS=ON \
    -DBUILD_TESTING=ON \
    -DCMAKE_BUILD_TYPE=Release

cmake --build "$BUILD_DIR" --parallel "$JOBS"
ctest --test-dir "$BUILD_DIR" --output-on-failure

{
    printf 'qualification=native-e2k-linux\n'
    printf 'uname=%s\n' "$(uname -a)"
    printf 'compiler=%s\n' "$CC"
    printf 'compiler_version=%s\n' "$("$CC" --version 2>&1 | head -n 1 || true)"
    printf 'source_commit=%s\n' "${GITHUB_SHA:-local}"
    printf 'result=PASS\n'
} > "$BUILD_DIR/e2k-ecosystem-qualification.txt"

echo "[NUN] Integrated E2K CTest suite passed."
echo "[NUN] Evidence: $BUILD_DIR/e2k-ecosystem-qualification.txt"
