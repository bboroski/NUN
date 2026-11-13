#!/usr/bin/env python3
"""Build and run the NUN E2K reproducible microbenchmark on native hardware."""

from __future__ import annotations

import csv
import datetime as dt
import json
import os
import platform
import shlex
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build" / "e2k" / "benchmark"
SOURCE = ROOT / "benchmarks" / "e2k_microbench.c"
BINARY = BUILD / "nun-e2k-microbench"
RESULTS = BUILD / "results"


def run(command: list[str], **kwargs) -> subprocess.CompletedProcess:
    print("+", shlex.join(command), flush=True)
    return subprocess.run(command, check=True, text=True, **kwargs)


def main() -> int:
    if platform.machine() != "e2k" and os.environ.get("NUN_E2K_ALLOW_USER_EMULATION") != "1":
        print("Refusing benchmark: native E2K Linux required. User-mode emulation must be explicitly marked.", file=sys.stderr)
        return 3

    compiler = shlex.split(os.environ.get("E2K_CC", "lcc"))
    if not compiler:
        print("E2K_CC must name the MCST LCC compiler.", file=sys.stderr)
        return 2
    version = subprocess.run(compiler + ["--version"], check=True, text=True,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.splitlines()[0]
    flags = ["-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
             "-D_POSIX_C_SOURCE=200809L"]
    BUILD.mkdir(parents=True, exist_ok=True)
    RESULTS.mkdir(parents=True, exist_ok=True)
    run(compiler + flags + [str(SOURCE), "-o", str(BINARY)])

    iterations = int(os.environ.get("NUN_E2K_BENCH_ITERATIONS", "2000000"))
    repeats = int(os.environ.get("NUN_E2K_BENCH_REPEATS", "5"))
    elements = int(os.environ.get("NUN_E2K_BENCH_ELEMENTS", "262144"))
    if iterations < 1 or repeats < 2 or elements < 1024:
        print("Require iterations>=1, repeats>=2 and elements>=1024.", file=sys.stderr)
        return 2

    completed = run([str(BINARY), str(iterations), str(repeats), str(elements)],
                    stdout=subprocess.PIPE)
    csv_path = RESULTS / "microbench.csv"
    csv_path.write_text(completed.stdout, encoding="utf-8")
    metadata = {
        "schema_version": "nun.e2k.benchmark.v1",
        "status": "native-e2k-tested" if platform.machine() == "e2k" else "emulated",
        "timestamp_utc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "target": {
            "machine": platform.machine(),
            "system": platform.system(),
            "release": platform.release(),
            "version": platform.version(),
            "processor": platform.processor(),
            "python": platform.python_version(),
        },
        "compiler": {"command": compiler, "version": version, "flags": flags},
        "parameters": {"iterations": iterations, "repeats": repeats,
                       "elements": elements, "memory_bytes": elements * 16},
        "source": {"path": str(SOURCE.relative_to(ROOT)),
                   "sha256": __import__("hashlib").sha256(SOURCE.read_bytes()).hexdigest(),
                   "commit": os.environ.get("GITHUB_SHA", "local")},
        "artifacts": {"csv": csv_path.name},
        "limitations": [
            "User-space microbenchmarks; not a full-system benchmark.",
            "No hardware performance counters or energy measurements are collected.",
            "Compare only runs with matching target, compiler, flags and parameters.",
            "The pointer-chase working set is one sequential cycle; it is not a cache-latency characterization suite."
        ],
    }
    (RESULTS / "metadata.json").write_text(
        json.dumps(metadata, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"CSV: {csv_path}")
    print(f"Metadata: {RESULTS / 'metadata.json'}")
    print("Classification:", metadata["status"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
