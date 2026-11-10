#!/usr/bin/env python3
"""One-knob host contract checks for NUN; not a boot-image builder."""
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent


def check() -> None:
    build = ROOT / ".build"
    subprocess.run(
        ["cmake", "-S", str(ROOT), "-B", str(build), "-DBUILD_TESTING=ON"],
        check=True,
    )
    subprocess.run(["cmake", "--build", str(build), "--parallel"], check=True)
    subprocess.run(["ctest", "--test-dir", str(build), "--output-on-failure"], check=True)


def main() -> int:
    parser = argparse.ArgumentParser(description="NUN host contract validation")
    parser.add_argument("command", nargs="?", choices=("check",), default="check")
    parser.parse_args()
    try:
        check()
    except (OSError, subprocess.CalledProcessError) as exc:
        print(f"NUN check failed: {exc}", file=sys.stderr)
        return 1
    print("NUN handoff metadata contract: PASS (host-only)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
