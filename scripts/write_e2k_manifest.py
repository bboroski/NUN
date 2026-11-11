#!/usr/bin/env python3
"""Create a digest- and ABI-bound manifest for the NUN E2K Linux runtime."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import struct
import sys
from pathlib import Path


def inspect_e2k_elf(path: Path) -> dict:
    data = path.read_bytes()
    if len(data) < 64 or data[:4] != b"\x7fELF":
        raise ValueError("runtime binary is not a complete ELF image")
    if data[4] != 2 or data[5] != 1 or data[6] != 1:
        raise ValueError("E2K runtime must be ELF64 little-endian, version 1")
    e_type, machine, version, entry = struct.unpack_from("<HHIQ", data, 16)
    if machine != 175:
        raise ValueError(f"ELF e_machine is {machine}; expected EM_MCST_ELBRUS (175)")
    if version != 1 or e_type not in (2, 3) or entry == 0:
        raise ValueError("E2K ELF type, version or entry point is invalid")
    return {
        "format": "ELF64",
        "endianness": "little",
        "machine": "EM_MCST_ELBRUS",
        "e_machine": 175,
        "type": "ET_EXEC" if e_type == 2 else "ET_DYN",
        "entry_point": f"0x{entry:x}",
    }


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--compiler", default="LCC")
    parser.add_argument("--compiler-version", default="unknown")
    args = parser.parse_args()

    try:
        binary = args.binary.resolve()
        if not binary.is_file() or binary.stat().st_size == 0:
            raise ValueError("runtime binary must be a non-empty file")
        elf = inspect_e2k_elf(binary)
        document = {
            "schema_version": "nun.e2k.runtime.v1",
            "product": "NUN",
            "artifact": {
                "file": binary.name,
                "size_bytes": binary.stat().st_size,
                "sha256": sha256(binary),
            },
            "target": {
                "architecture": "e2k",
                "os": "linux",
                "word_bits": 64,
                "endianness": "little",
                "elf_machine": 175,
                "execution_mode": "native-linux-userspace",
                "deployment_chain": "MCST-PNS-to-Elbrus-Linux-to-NUN-userspace",
            },
            "binary": elf,
            "compiler": {
                "family": args.compiler,
                "version": args.compiler_version,
            },
            "security": {
                "digest_algorithm": "sha256",
                "signature_status": "unsigned",
            },
            "build": {
                "source_commit": os.environ.get("GITHUB_SHA", "local"),
            },
        }
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(
            json.dumps(document, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
    except (OSError, ValueError) as exc:
        print(f"NUN E2K manifest error: {exc}", file=sys.stderr)
        return 1

    print(f"NUN E2K runtime manifest written: {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
