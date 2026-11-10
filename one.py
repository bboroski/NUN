#!/usr/bin/env python3
"""One-knob NUN superbuild: native modules, integrated tests and host tooling."""

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
MODULES = ("SAMBU", "ToolNG", "ReaperCripple", "Kloz", "CO", "SaintSI", "Klipot")


def run(command: list[str], *, cwd: Path = ROOT) -> None:
    print("+", " ".join(command), flush=True)
    subprocess.run(command, cwd=cwd, check=True)


def ensure_sources() -> None:
    missing = []
    for name in MODULES:
        path = ROOT / "modules" / name
        if not path.exists() or not any(path.iterdir()):
            missing.append(name)
    if missing:
        git = shutil.which("git")
        if git is None:
            raise SystemExit("Git is required. Clone NUN with --recurse-submodules.")
        run([git, "submodule", "update", "--init", "--recursive"])
    still_missing = [name for name in MODULES
                     if not (ROOT / "modules" / name / "sambu.module.json").is_file()]
    if still_missing:
        raise SystemExit("Missing source modules: " + ", ".join(still_missing) +
                         ". Run: git submodule update --init --recursive")


def main() -> int:
    ensure_sources()
    cmake = shutil.which("cmake")
    if cmake is None:
        raise SystemExit("CMake 3.20+ is required.")
    build_dir = ROOT / "build"
    run([cmake, "-S", str(ROOT), "-B", str(build_dir),
         "-DNUN_BUILD_TESTS=ON"])
    run([cmake, "--build", str(build_dir), "--config", "Release"])
    ctest = shutil.which("ctest")
    if ctest:
        run([ctest, "--test-dir", str(build_dir), "--output-on-failure",
             "-C", "Release"])

    python = sys.executable
    pytest = subprocess.run([python, "-m", "pytest", "--version"],
                            cwd=ROOT, stdout=subprocess.DEVNULL,
                            stderr=subprocess.DEVNULL, check=False)
    if pytest.returncode == 0:
        run([python, "-m", "pytest", "-q",
             str(ROOT / "modules" / "SAMBU" / "tests"),
             str(ROOT / "modules" / "Klipot" / "tests")])
    else:
        print("SKIP Python suites: install pytest to run SAMBU and Klipot tests.")
    print("NUN host integration build completed. No bootable OS image is produced.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except subprocess.CalledProcessError as exc:
        raise SystemExit(exc.returncode)
