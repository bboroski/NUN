import importlib.util
import tempfile
import unittest
from pathlib import Path
import struct

SCRIPT = Path(__file__).resolve().parents[1] / "scripts" / "write_image_manifest.py"
SPEC = importlib.util.spec_from_file_location("nun_image_manifest", SCRIPT)
MODULE = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
SPEC.loader.exec_module(MODULE)


def make_elf(path: Path) -> None:
    data = bytearray(120)
    data[:4] = b"\x7fELF"
    data[4] = 2
    data[5] = 1
    data[6] = 1
    struct.pack_into("<HHIQ", data, 16, 3, 62, 1, 0x400080)
    struct.pack_into("<Q", data, 32, 64)  # e_phoff
    struct.pack_into("<HHH", data, 52, 64, 56, 1)  # ELF header / PHDR size / count
    struct.pack_into("<IIQQQQQQ", data, 64, 1, 5, 0, 0x400000, 0, len(data), 0x1000, 0x1000)
    path.write_bytes(data)


class ImageManifestTests(unittest.TestCase):
    def test_manifest_records_digest_and_only_verified_x86_target(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            image = root / "NUN.iso"
            kernel = root / "kernel.elf"
            image.write_bytes(b"synthetic ISO image")
            make_elf(kernel)

            result = MODULE.create_manifest(
                image, kernel, "12.9.6",
                "f00bf1e994e969905b3a8259e2414809174491a8e8ae87e7129095745dfc6857",
                "test-commit",
            )
            self.assertEqual(result["artifact"]["size_bytes"], image.stat().st_size)
            self.assertEqual(result["target"]["architecture"], "x86_64")
            self.assertEqual(result["target"]["firmware"], ["bios", "uefi"])
            self.assertEqual(result["kernel"]["machine"], "x86_64")
            self.assertEqual(result["kernel"]["type"], "ET_DYN")
            self.assertEqual(result["security"]["signature_status"], "unsigned")

    def test_rejects_non_elf_kernel(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            image = root / "NUN.iso"
            kernel = root / "kernel.elf"
            image.write_bytes(b"image")
            kernel.write_bytes(b"not an elf")
            with self.assertRaisesRegex(ValueError, "ELF"):
                MODULE.create_manifest(
                    image, kernel, "12.9.6",
                    "f00bf1e994e969905b3a8259e2414809174491a8e8ae87e7129095745dfc6857",
                )

    def test_rejects_invalid_upstream_digest(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            image = root / "NUN.iso"
            kernel = root / "kernel.elf"
            image.write_bytes(b"image")
            make_elf(kernel)
            with self.assertRaisesRegex(ValueError, "SHA-256"):
                MODULE.create_manifest(image, kernel, "12.9.6", "bad")


if __name__ == "__main__":
    unittest.main()
