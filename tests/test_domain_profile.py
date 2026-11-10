import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from validate_domain_profile import validate_profile  # noqa: E402


class DomainProfileTests(unittest.TestCase):
    def setUp(self):
        self.profile = json.loads(
            (ROOT / "spec" / "domain-capability-profile.json").read_text(encoding="utf-8")
        )

    def test_reference_profile_is_valid(self):
        self.assertEqual(validate_profile(self.profile), [])

    def test_duplicate_capability_is_rejected(self):
        self.profile["capabilities"].append(dict(self.profile["capabilities"][0]))
        self.assertTrue(any("duplicate capability id" in item for item in validate_profile(self.profile)))

    def test_fictional_physics_must_remain_simulation_only(self):
        self.profile["guardrails"]["fictional_physics_is_simulation_only"] = False
        self.assertTrue(any("fictional_physics" in item for item in validate_profile(self.profile)))

    def test_missing_evidence_must_not_be_promoted(self):
        self.profile["guardrails"]["missing_evidence_means_unknown"] = False
        self.assertTrue(any("missing_evidence" in item for item in validate_profile(self.profile)))


if __name__ == "__main__":
    unittest.main()
