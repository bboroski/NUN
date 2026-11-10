#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/build/include/limine.h"
COMMIT="6e85ddd6f928b5b512015c4d47d1cfce2fd28079"
EXPECTED="b38a3f35e2a3d547f80326a13279f153aee8a837"
URL="https://raw.githubusercontent.com/Limine-Bootloader/limine-protocol/$COMMIT/include/limine.h"
command -v curl >/dev/null
command -v git >/dev/null
mkdir -p "$(dirname "$OUT")"
curl --fail --location --silent --show-error "$URL" -o "$OUT"
ACTUAL="$(git hash-object "$OUT")"
if [[ "$ACTUAL" != "$EXPECTED" ]]; then
  echo "Limine protocol header identity mismatch: $ACTUAL" >&2
  rm -f "$OUT"
  exit 1
fi
echo "Verified Limine protocol header at $COMMIT"
