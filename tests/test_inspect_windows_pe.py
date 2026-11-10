from __future__ import annotations

import struct
import unittest

from tools.inspect_windows_pe import PEFormatError, inspect_pe


def make_pe(
    *,
    pe32_plus: bool = True,
    imports: list[str] | None = None,
    bad_raw_range: bool = False,
    bad_magic: bool = False,
) -> bytes:
    pe_offset = 0x80
    optional_size = 0xF0 if pe32_plus else 0xE0
    optional = bytearray(optional_size)
    struct.pack_into("<H", optional, 0, 0x20B if pe32_plus and not bad_magic else 0x10B)
    struct.pack_into("<I", optional, 16, 0x1000)
    if pe32_plus:
        struct.pack_into("<Q", optional, 24, 0x140000000)
    else:
        struct.pack_into("<I", optional, 28, 0x400000)
    struct.pack_into("<II", optional, 32, 0x1000, 0x200)
    struct.pack_into("<II", optional, 56, 0x2000, 0x200)
    struct.pack_into("<H", optional, 68, 2)
    dir_count_offset = 108 if pe32_plus and not bad_magic else 92
    dir_table_offset = 112 if pe32_plus and not bad_magic else 96
    struct.pack_into("<I", optional, dir_count_offset, 16)

    payload = bytearray(0x400)
    payload[:2] = b"MZ"
    struct.pack_into("<I", payload, 0x3C, pe_offset)
    payload[pe_offset:pe_offset + 4] = b"PE\0\0"
    machine = 0x8664 if pe32_plus and not bad_magic else 0x14C
    if bad_magic:
        machine = 0x8664
    struct.pack_into(
        "<HHIIIHH",
        payload,
        pe_offset + 4,
        machine,
        1,
        0x12345678,
        0,
        0,
        optional_size,
        0x0002,
    )
    optional_offset = pe_offset + 24
    payload[optional_offset:optional_offset + optional_size] = optional
    section_offset = optional_offset + optional_size
    raw_size = 0x200
    raw_pointer = 0x200
    if bad_raw_range:
        raw_pointer = 0x3F0
    struct.pack_into(
        "<8sIIIIIIHHI",
        payload,
        section_offset,
        b".rdata\0\0",
        0x200,
        0x1000,
        raw_size,
        raw_pointer,
        0,
        0,
        0,
        0,
        0x40000040,
    )

    if imports:
        import_directory = raw_pointer
        name_offset = raw_pointer + 0x40
        import_rva = 0x1000
        import_size = 20 * (len(imports) + 1)
        struct.pack_into(
            "<II",
            payload,
            optional_offset + dir_table_offset + 8,
            import_rva,
            import_size,
        )
        for index, name in enumerate(imports):
            descriptor_offset = import_directory + index * 20
            name_rva = 0x1000 + (name_offset - raw_pointer) + index * 0x40
            struct.pack_into(
                "<IIIII", payload, descriptor_offset, 0, 0, 0, name_rva, 0
            )
            encoded = name.encode("ascii") + b"\0"
            start = name_offset + index * 0x40
            payload[start:start + len(encoded)] = encoded
    return bytes(payload)


class InspectWindowsPETests(unittest.TestCase):
    def test_pe32_plus_x64(self) -> None:
        result = inspect_pe(make_pe())
        self.assertEqual(result["format"], "PE32+")
        self.assertEqual(result["machine"], "x86_64")
        self.assertEqual(result["subsystem"], "windows_gui")
        self.assertTrue(result["initial_x86_x64_target"])

    def test_pe32_x86(self) -> None:
        result = inspect_pe(make_pe(pe32_plus=False))
        self.assertEqual(result["format"], "PE32")
        self.assertEqual(result["machine"], "x86")

    def test_import_dll_names(self) -> None:
        result = inspect_pe(make_pe(imports=["KERNEL32.dll", "USER32.dll"]))
        self.assertEqual(result["imports"], ["KERNEL32.dll", "USER32.dll"])

    def test_rejects_non_pe_file(self) -> None:
        with self.assertRaises(PEFormatError):
            inspect_pe(b"not a PE file".ljust(128, b"\0"))

    def test_rejects_out_of_bounds_section(self) -> None:
        with self.assertRaises(PEFormatError):
            inspect_pe(make_pe(bad_raw_range=True))

    def test_rejects_machine_optional_header_mismatch(self) -> None:
        with self.assertRaises(PEFormatError):
            inspect_pe(make_pe(bad_magic=True))

    def test_rejects_truncated_header(self) -> None:
        with self.assertRaises(PEFormatError):
            inspect_pe(b"MZ" + b"\0" * 40)


if __name__ == "__main__":
    unittest.main()
