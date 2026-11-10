#!/usr/bin/env python3
"""Validate NUN Windows-game compatibility planning metadata.

This checks schemas and declarative profile/matrix structure only. It does
not parse or execute PE files and does not prove that any game can run.
"""

from __future__ import annotations

import csv
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_FILE = ROOT / "spec" / "windows-game-profile.schema.json"
PROFILE_FILE = ROOT / "data" / "compatibility" / "windows-game-profile.template.json"
MATRIX_FILE = ROOT / "data" / "compatibility" / "windows-game-capability-matrix.tsv"

EXPECTED_MATRIX_FIELDS = [
    "capability_id",
    "feature_group",
    "initial_scope",
    "required_nun_contract",
    "acceptance_evidence",
    "implementation_status",
]
ALLOWED_STATUSES = {"not_implemented", "in_progress", "implemented", "verified"}
BLOCKED_ANTI_CHEAT = {"kernel_mode"}


def load_json(path: Path, errors: list[str]) -> Any:
    try:
        with path.open("r", encoding="utf-8") as handle:
            return json.load(handle)
    except (OSError, UnicodeError, json.JSONDecodeError) as exc:
        errors.append(f"{path.relative_to(ROOT)}: cannot read valid JSON: {exc}")
        return None


def validate_schema(schema: Any, errors: list[str]) -> None:
    if not isinstance(schema, dict):
        errors.append("Profile schema must be a JSON object")
        return

    if schema.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
        errors.append("Profile schema must use JSON Schema draft 2020-12")
    if schema.get("$id") != "urn:nun:windows-game-profile:1":
        errors.append("Unexpected Windows game profile schema identifier")

    required = schema.get("required")
    properties = schema.get("properties")
    if not isinstance(required, list) or not isinstance(properties, dict):
        errors.append("Profile schema must declare required fields and properties")
        return

    missing = sorted(set(required) - set(properties))
    if missing:
        errors.append(f"Schema required fields lack property definitions: {missing}")

    for field in ("profile_version", "id", "executable", "machine",
                  "subsystem", "graphics", "audio", "input", "security",
                  "support_tier"):
        if field not in required:
            errors.append(f"Schema is missing required top-level field: {field}")

    if schema.get("additionalProperties") is not False:
        errors.append("Top-level profile schema must reject undeclared fields")


def validate_profile(profile: Any, schema: Any, errors: list[str]) -> None:
    if not isinstance(profile, dict) or not isinstance(schema, dict):
        errors.append("Profile template and schema must be JSON objects")
        return

    required = schema.get("required", [])
    missing = [field for field in required if field not in profile]
    if missing:
        errors.append(f"Profile template is missing required fields: {missing}")
        return

    if profile.get("profile_version") != 1:
        errors.append("Profile template must use profile_version 1")

    properties = schema.get("properties", {})
    for field in ("machine", "subsystem", "support_tier"):
        allowed = properties.get(field, {}).get("enum", [])
        if profile.get(field) not in allowed:
            errors.append(f"Profile template has invalid {field}: {profile.get(field)!r}")

    security = profile.get("security")
    if not isinstance(security, dict):
        errors.append("Profile security section must be an object")
    else:
        anti_cheat = security.get("anti_cheat")
        kernel_driver = security.get("kernel_driver_required")
        if (anti_cheat in BLOCKED_ANTI_CHEAT or kernel_driver is True) and profile.get("support_tier") != "blocked":
            errors.append("Kernel-mode anti-cheat/driver profiles must be marked blocked")

    executable = profile.get("executable")
    if not isinstance(executable, dict) or not isinstance(executable.get("path"), str) or not executable["path"].strip():
        errors.append("Profile executable.path must be a non-empty string")

    for section in ("graphics", "audio", "input"):
        value = profile.get(section)
        if not isinstance(value, dict):
            errors.append(f"Profile {section} section must be an object")
        elif not isinstance(value.get("apis"), list):
            errors.append(f"Profile {section}.apis must be an array")


def validate_matrix(errors: list[str]) -> None:
    try:
        with MATRIX_FILE.open("r", encoding="utf-8", newline="") as handle:
            reader = csv.DictReader(handle, delimiter="\t")
            if reader.fieldnames != EXPECTED_MATRIX_FIELDS:
                errors.append(
                    "Capability matrix header mismatch: expected "
                    + "|".join(EXPECTED_MATRIX_FIELDS)
                )
                return
            rows = list(reader)
    except (OSError, UnicodeError, csv.Error) as exc:
        errors.append(f"Capability matrix cannot be read: {exc}")
        return

    if not rows:
        errors.append("Capability matrix must contain at least one capability")
        return

    seen: set[str] = set()
    for line_number, row in enumerate(rows, start=2):
        capability_id = (row.get("capability_id") or "").strip()
        if not capability_id:
            errors.append(f"Matrix line {line_number}: empty capability_id")
        elif capability_id in seen:
            errors.append(f"Matrix line {line_number}: duplicate capability_id {capability_id}")
        seen.add(capability_id)

        for field in EXPECTED_MATRIX_FIELDS:
            if not (row.get(field) or "").strip():
                errors.append(f"Matrix line {line_number}: empty {field}")

        status = (row.get("implementation_status") or "").strip()
        if status not in ALLOWED_STATUSES:
            errors.append(f"Matrix line {line_number}: invalid status {status!r}")

    # This repository is specification-only. Prevent a planning matrix from
    # accidentally advertising implementation or verification evidence.
    premature = [
        row["capability_id"]
        for row in rows
        if row.get("implementation_status") in {"implemented", "verified"}
    ]
    if premature:
        errors.append(
            "Specification-only NUN cannot mark capabilities implemented/verified: "
            + ", ".join(premature)
        )


def main() -> int:
    errors: list[str] = []
    schema = load_json(SCHEMA_FILE, errors)
    profile = load_json(PROFILE_FILE, errors)

    validate_schema(schema, errors)
    validate_profile(profile, schema, errors)
    validate_matrix(errors)

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        return 1

    print(
        "OK: Windows game profile schema, template and capability matrix are "
        "structurally consistent. Runtime support is not implemented."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
