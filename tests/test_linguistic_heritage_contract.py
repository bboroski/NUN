import json
import unittest
from pathlib import Path

RECORD = Path(__file__).resolve().parents[1] / "data" / "linguistic-heritage" / "cyrillic-letter-y.json"

class LinguisticHeritageContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.record = json.loads(RECORD.read_text(encoding="utf-8"))

    def test_identity(self):
        self.assertEqual(self.record["schema_version"], "linguistic-heritage.letter.v1")
        self.assertEqual((self.record["grapheme"], self.record["lowercase"]), ("Ы", "ы"))

    def test_graphics_and_sound_are_separate(self):
        self.assertIn("historical_graphics", self.record)
        self.assertIn("historical_phonology", self.record)
        self.assertIn("phoneme_profile", self.record)

    def test_uncertainty_is_explicit(self):
        self.assertIn("confidence", self.record["historical_graphics"])
        self.assertEqual(self.record["interpretive_layer"]["status"], "cultural_interpretation_not_historical_evidence")
        self.assertTrue(self.record["claims_policy"]["label_hypotheses_and_metaphors"])

    def test_slavic_comparison_avoids_equivalence(self):
        self.assertIn("not identical", self.record["comparative_slavic"]["polish_y"])
        self.assertIn("not an exact equivalent", self.record["comparative_slavic"]["ukrainian_y"])

    def test_references(self):
        self.assertGreaterEqual(len(self.record["references"]), 2)
        self.assertTrue(all(x["url"].startswith("https://") for x in self.record["references"]))

if __name__ == "__main__":
    unittest.main()
