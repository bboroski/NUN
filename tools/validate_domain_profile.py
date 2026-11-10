#!/usr/bin/env python3
"""Validate NUN's versioned domain capability specification."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

DOMAINS = {
    "biosynthetic", "physics", "defense", "cycles",
    "synthetic-intelligence", "genetics-and-evolution",
}
STATUSES = {"specified", "prototype", "implemented", "verified"}


def validate_profile(document: Any) -> list[str]:
    errors: list[str] = []
    if not isinstance(document, dict):
        return ["profile root must be an object"]
    if document.get("schema_version") != "nun.domain-capabilities.v1":
        errors.append("unsupported schema_version")
    if not isinstance(document.get("profile_id"), str) or not document["profile_id"].strip():
        errors.append("profile_id must be a non-empty string")
    if document.get("profile_status") not in ("specification", "prototype", "implementation"): 
        errors.append("profile_status is invalid")
    capabilities = document.get("capabilities")
    if not isinstance(capabilities, list) or not capabilities:
        return errors + ["capabilities must be a non-empty array"]

    seen: set[str] = set()
    for index, item in enumerate(capabilities):
        prefix = f"capabilities[{index}]"
        if not isinstance(item, dict):
            errors.append(f"{prefix} must be an object")
            continue
        identifier = item.get("id")
        if not isinstance(identifier, str) or not identifier.strip():
            errors.append(f"{prefix}.id must be non-empty")
        elif identifier in seen:
            errors.append(f"duplicate capability id: {identifier}")
        else:
            seen.add(identifier)
        if item.get("domain") not in tuple(DOMAINS):
            errors.append(f"{prefix}.domain is unsupported")
        for field in ("interface",):
            if not isinstance(item.get(field), str) or not item[field].strip():
                errors.append(f"{prefix}.{field} must be non-empty")
        if item.get("status") not in tuple(STATUSES):
            errors.append(f"{prefix}.status is unsupported")
        for field in ("maps_terms", "permissions"):
            values = item.get(field)
            if not isinstance(values, list) or not values or any(
                not isinstance(value, str) or not value.strip() for value in values
            ):
                errors.append(f"{prefix}.{field} must be a non-empty string array")

    guardrails = document.get("guardrails")
    expected = {
        "glossary_is_executable": False,
        "fictional_physics_is_simulation_only": True,
        "biological_terms_are_not_lab_instructions": True,
        "missing_evidence_means_unknown": True,
        "privileged_actions_require_capabilities": True,
        "activation_requires_rollback_path": True,
    }
    if not isinstance(guardrails, dict):
        errors.append("guardrails must be an object")
    else:
        for key, expected_value in expected.items():
            if guardrails.get(key) is not expected_value:
                errors.append(f"guardrails.{key} must be {expected_value!r}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "profile", nargs="?",
        default=str(Path(__file__).resolve().parents[1] / "spec" / "domain-capability-profile.json"),
    )
    args = parser.parse_args()
    try:
        document = json.loads(Path(args.profile).read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        print(f"ERROR: {exc}")
        return 2
    errors = validate_profile(document)
    if errors:
        for error in errors:
            print(f"ERROR: {error}")
        return 1
    print("NUN domain capability profile: VALID (specification only)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
