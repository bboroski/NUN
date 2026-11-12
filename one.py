#!/usr/bin/env python3
"""One-knob NUN superbuild: native modules, pinned integrations and host tooling."""

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
CORE_MODULES = ("SAMBU", "ToolNG", "ReaperCripple", "Kloz", "CO", "SaintSI", "Klipot")
SUBMODULE_PATHS = (
    *(f"modules/{name}" for name in CORE_MODULES),
    "modules/NUNLauncher",
    "modules/Luci",
    "modules/Fragile-Baby",
    "federation/Hellduck",
)


def run(command: list[str], *, cwd: Path = ROOT) -> None:
    print("+", " ".join(command), flush=True)
    subprocess.run(command, cwd=cwd, check=True)


def ensure_sources() -> None:
    missing = [relative for relative in SUBMODULE_PATHS
               if not (ROOT / relative).is_dir() or not any((ROOT / relative).iterdir())]
    if missing:
        git = shutil.which("git")
        if git is None:
            raise SystemExit("Git is required. Clone NUN with --recurse-submodules.")
        run([git, "submodule", "update", "--init", "--recursive"])
    missing = [relative for relative in SUBMODULE_PATHS
               if not (ROOT / relative).is_dir() or not any((ROOT / relative).iterdir())]
    if missing:
        raise SystemExit("Missing pinned sources: " + ", ".join(missing) +
                         ". Run: git submodule update --init --recursive")
    invalid_core = [name for name in CORE_MODULES
                    if not (ROOT / "modules" / name / "sambu.module.json").is_file()]
    if invalid_core:
        raise SystemExit("Missing core module manifests: " + ", ".join(invalid_core))


def main() -> int:
    ensure_sources()
    cmake = shutil.which("cmake")
    if cmake is None:
        raise SystemExit("CMake 3.20+ is required.")
    build_dir = ROOT / "build"
    run([cmake, "-S", str(ROOT), "-B", str(build_dir), "-DNUN_BUILD_TESTS=ON"])
    run([cmake, "--build", str(build_dir), "--config", "Release"])
    ctest = shutil.which("ctest")
    if ctest:
        run([ctest, "--test-dir", str(build_dir), "--output-on-failure", "-C", "Release"])

    python = sys.executable
    # Luci remains a host-side, read-only registry and policy tool.
    run([python, "one.py", "check"], cwd=ROOT / "modules" / "Luci")
    # Fragile-Baby contributes evidence analysis only; no E2K target claim follows.
    run([python, "-m", "unittest", "discover", "-s", "tests", "-p", "test_*.py", "-v"],
        cwd=ROOT / "modules" / "Fragile-Baby")
    # Hellduck validates static federation metadata; it is never a boot dependency.
    run([python, "ecosystem/validate.py"], cwd=ROOT / "federation" / "Hellduck")

    pytest = subprocess.run([python, "-m", "pytest", "--version"],
                            cwd=ROOT, stdout=subprocess.DEVNULL,
                            stderr=subprocess.DEVNULL, check=False)
    if pytest.returncode == 0:
        run([python, "-m", "pytest", "-q", "tests"], cwd=ROOT / "modules" / "SAMBU")
        run([python, "-m", "pytest", "-q", "tests"], cwd=ROOT / "modules" / "Klipot")
    else:
        print("SKIP optional pytest suites: install pytest to run SAMBU and Klipot tests.")
    print("NUN host integration completed. This does not produce or certify a bootable OS.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except subprocess.CalledProcessError as exc:
        raise SystemExit(exc.returncode)
