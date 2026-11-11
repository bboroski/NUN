#!/usr/bin/env python3
"""Write a deterministic, machine-readable manifest for a NUN boot image."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import struct
import sys
from pathlib import Path


def inspect_kernel(path: Path) -> dict[str, str]:
    data = path.read_bytes()
    if len(data) < 64 or data[:4] != b"\x7fELF":
        raise ValueError("kernel is not a complete ELF image")
    if data[4] != 2 or data[5] != 1:
        raise ValueError("kernel must be ELF64 little-endian")
    e_type, machine, version, entry = struct.unpack_from("<HHIQ", data, 16)
    if version != 1 or machine != 62 or e_type != 3:
        raise ValueError("kernel must be an x86_64 ET_DYN ELF image")
    if entry == 0:
        raise ValueError("kernel entry point must not be zero")
    return {
        "format": "ELF64",
        "endianness": "little",
        "machine": "x86_64",
        "type": "ET_DYN",
        "entry_point": f"0x{entry:x}",
    }


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def create_manifest(
    image: Path,
    kernel: Path,
    limine_version: str,
    limine_sha256: str,
    source_commit: str = "local",
) -> dict:
    image = image.resolve()
    kernel = kernel.resolve()
    if not image.is_file() or image.stat().st_size == 0:
        raise ValueError("image must be a non-empty regular file")
    if not kernel.is_file() or kernel.stat().st_size == 0:
        raise ValueError("kernel must be a non-empty regular file")
    if len(limine_sha256) != 64 or any(c not in "0123456789abcdef" for c in limine_sha256.lower()):
        raise ValueError("Limine archive digest must be a 64-character SHA-256 hex string")

    kernel_info = inspect_kernel(kernel)
    return {
        "schema_version": 1,
        "product": "NUN",
        "artifact": {
            "file": image.name,
            "format": "ISO9660-hybrid",
            "size_bytes": image.stat().st_size,
            "sha256": sha256(image),
        },
        "target": {
            "architecture": "x86_64",
            "firmware": ["bios", "uefi"],
            "boot_protocol": "limine",
            "limine_base_revision": 6,
        },
        "bootloader": {
            "name": "Limine",
            "version": limine_version,
            "archive_sha256": limine_sha256.lower(),
        },
        "kernel": {
            "path": "boot/NUN/kernel.elf",
            **kernel_info,
        },
        "security": {
            "digest_algorithm": "sha256",
            "signature_status": "unsigned",
        },
        "build": {
            "source_commit": source_commit,
        },
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--image", required=True, type=Path)
    parser.add_argument("--kernel", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--limine-version", required=True)
    parser.add_argument("--limine-sha256", required=True)
    args = parser.parse_args(argv)

    try:
        manifest = create_manifest(
            args.image,
            args.kernel,
            args.limine_version,
            args.limine_sha256,
            os.environ.get("GITHUB_SHA", "local"),
        )
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(
            json.dumps(manifest, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
    except (OSError, ValueError) as exc:
        print(f"NUN manifest error: {exc}", file=sys.stderr)
        return 1
    print(f"NUN image manifest written: {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
