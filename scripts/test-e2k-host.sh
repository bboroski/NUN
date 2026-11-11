#!/usr/bin/env bash
set -Eeuo pipefail
ROOT="$(cd -- "$(dirname -- "$0")/.." && pwd -P)"
BINARY="$ROOT/build/e2k/NUN-E2K-host"
if [[ ! -x "$BINARY" ]]; then
    echo "Missing E2K runtime. Run make e2k-host first." >&2
    exit 2
fi
if [[ "$(uname -m)" != "e2k" && "${NUN_E2K_ALLOW_USER_EMULATION:-0}" != "1" ]]; then
    echo "Native E2K runtime test requires uname -m=e2k." >&2
    echo "For qemu-e2k-user only, set NUN_E2K_ALLOW_USER_EMULATION=1." >&2
    exit 3
fi
"$BINARY" --self-test
