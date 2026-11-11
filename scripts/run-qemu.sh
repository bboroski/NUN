#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ISO="${1:-$ROOT/build/qemu/nun-qemu.iso}"
exec qemu-system-x86_64 -machine q35 -m 512M -cdrom "$ISO" -boot d -display none -serial stdio -monitor none -no-reboot -no-shutdown
