#!/usr/bin/env python3
"""Inspect Windows PE executables without loading or executing them.

This is a host-side Stage 1 diagnostic tool for NUN. It is not a PE loader,
Windows API implementation, CPU emulator, or game launcher.
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path
from typing import Any

DOS_HEADER_SIZE = 64
PE_SIGNATURE = b"PE\0\0"
COFF_HEADER_SIZE = 20
SECTION_HEADER_SIZE = 40
MAX_SECTIONS = 96
MAX_IMPORTS = 4096
MAX_DLL_NAME = 4096

MACHINE_NAMES = {
    0x014C: "x86",
    0x8664: "x86_64",
    0xAA64: "arm64",
    0x01C0: "arm",
    0x01C4: "armv7",
    0x0200: "ia64",
}
SUBSYSTEM_NAMES = {
    0: "unknown",
    1: "native",
    2: "windows_gui",
    3: "windows_console",
    7: "posix_console",
    9: "windows_ce_gui",
    10: "efi_application",
    11: "efi_boot_service_driver",
    12: "efi_runtime_driver",
    13: "efi_rom",
    14: "xbox",
    16: "windows_boot_application",
}


class PEFormatError(ValueError):
    """Raised when a PE file is malformed or internally inconsistent."""


def _slice(data: bytes, offset: int, size: int, label: str) -> bytes:
    if offset < 0 or size < 0 or offset > len(data) or size > len(data) - offset:
        raise PEFormatError(f"{label} is outside the file bounds")
    return data[offset : offset + size]


def _unpack(fmt: str, data: bytes, offset: int, label: str) -> tuple[Any, ...]:
    size = struct.calcsize(fmt)
    try:
        return struct.unpack(fmt, _slice(data, offset, size, label))
    except struct.error as exc:
        raise PEFormatError(f"Invalid {label}: {exc}") from exc


def _decode_name(raw: bytes) -> str:
    return raw.split(b"\0", 1)[0].decode("ascii", errors="replace")


def _rva_to_offset(
    rva: int,
    size: int,
    sections: list[dict[str, Any]],
    size_headers: int,
    data_size: int,
) -> int:
    if rva < 0 or size < 0 or rva > 0xFFFFFFFF or size > 0xFFFFFFFF - rva:
        raise PEFormatError("RVA range overflows the PE address space")

    if rva < size_headers:
        if rva + size > size_headers or rva + size > data_size:
            raise PEFormatError("Header RVA range is outside the file")
        return rva

    for section in sections:
        start = section["virtual_address"]
        span = max(section["virtual_size"], section["raw_size"])
        if start <= rva < start + span:
            delta = rva - start
            if delta + size > section["raw_size"]:
                raise PEFormatError("RVA points into an unbacked section range")
            offset = section["raw_pointer"] + delta
            if offset > data_size or size > data_size - offset:
                raise PEFormatError("Mapped RVA range is outside the file")
            return offset

    raise PEFormatError(f"RVA 0x{rva:08X} is not mapped by headers or sections")


def _read_c_string_at_rva(
    data: bytes,
    rva: int,
    sections: list[dict[str, Any]],
    size_headers: int,
    label: str,
) -> str:
    offset = _rva_to_offset(rva, 1, sections, size_headers, len(data))
    end = min(len(data), offset + MAX_DLL_NAME)
    terminator = data.find(b"\0", offset, end)
    if terminator < 0:
        raise PEFormatError(f"{label} is not null-terminated within {MAX_DLL_NAME} bytes")
    if terminator == offset:
        raise PEFormatError(f"{label} is empty")
    return data[offset:terminator].decode("ascii", errors="replace")


def _read_imports(
    data: bytes,
    import_rva: int,
    import_size: int,
    sections: list[dict[str, Any]],
    size_headers: int,
) -> list[str]:
    if import_rva == 0 and import_size == 0:
        return []
    if import_rva == 0 or import_size == 0 or import_size < 20:
        raise PEFormatError("Import directory has an invalid RVA/size pair")

    directory_offset = _rva_to_offset(
        import_rva, min(import_size, 20), sections, size_headers, len(data)
    )
    descriptor_count = min(import_size // 20, MAX_IMPORTS)
    imports: list[str] = []
    terminated = False

    for index in range(descriptor_count):
        descriptor = _unpack(
            "<IIIII", data, directory_offset + index * 20, "import descriptor"
        )
        if not any(descriptor):
            terminated = True
            break
        name_rva = descriptor[3]
        if name_rva == 0:
            raise PEFormatError("Import descriptor has no DLL name RVA")
        imports.append(
            _read_c_string_at_rva(
                data, name_rva, sections, size_headers, "import DLL name"
            )
        )

    if not terminated:
        raise PEFormatError(
            "Import directory has no null descriptor within its declared size"
        )
    return sorted(set(imports), key=str.casefold)


def inspect_pe(data: bytes) -> dict[str, Any]:
    """Return bounded PE metadata; never maps code or executes the input."""
    if len(data) < DOS_HEADER_SIZE:
        raise PEFormatError("File is too small for a DOS header")
    if data[:2] != b"MZ":
        raise PEFormatError("Missing DOS MZ signature")

    (pe_offset,) = _unpack("<I", data, 0x3C, "DOS e_lfanew")
    if pe_offset < DOS_HEADER_SIZE:
        raise PEFormatError("PE header offset overlaps the DOS header")
    if _slice(data, pe_offset, 4, "PE signature") != PE_SIGNATURE:
        raise PEFormatError("Missing PE signature at e_lfanew")

    coff_offset = pe_offset + 4
    (
        machine,
        number_sections,
        timestamp,
        _symbol_pointer,
        _symbol_count,
        optional_size,
        characteristics,
    ) = _unpack("<HHIIIHH", data, coff_offset, "COFF header")
    if number_sections == 0 or number_sections > MAX_SECTIONS:
        raise PEFormatError(f"Unsupported section count: {number_sections}")

    optional_offset = coff_offset + COFF_HEADER_SIZE
    optional = _slice(data, optional_offset, optional_size, "optional header")
    if len(optional) < 2:
        raise PEFormatError("Optional header is missing its magic")
    (magic,) = struct.unpack_from("<H", optional, 0)

    if magic == 0x10B:
        format_name = "PE32"
        image_base = struct.unpack_from("<I", optional, 28)[0] if optional_size >= 32 else None
        directory_count_offset = 92
        directory_table_offset = 96
    elif magic == 0x20B:
        format_name = "PE32+"
        image_base = struct.unpack_from("<Q", optional, 24)[0] if optional_size >= 32 else None
        directory_count_offset = 108
        directory_table_offset = 112
    else:
        raise PEFormatError(f"Unsupported optional-header magic: 0x{magic:04X}")

    if machine == 0x014C and magic != 0x10B:
        raise PEFormatError("x86 machine type must use a PE32 optional header")
    if machine in (0x8664, 0xAA64, 0x0200) and magic != 0x20B:
        raise PEFormatError("64-bit machine type must use a PE32+ optional header")
    if machine not in MACHINE_NAMES:
        machine_name = f"unknown_0x{machine:04X}"
    else:
        machine_name = MACHINE_NAMES[machine]

    if optional_size < directory_table_offset:
        raise PEFormatError("Optional header is shorter than its fixed fields")

    entry_point = struct.unpack_from("<I", optional, 16)[0]
    section_alignment, file_alignment = struct.unpack_from("<II", optional, 32)
    size_of_image, size_of_headers = struct.unpack_from("<II", optional, 56)
    subsystem = struct.unpack_from("<H", optional, 68)[0]
    number_of_directories = struct.unpack_from("<I", optional, directory_count_offset)[0]
    available_directories = (optional_size - directory_table_offset) // 8
    if number_of_directories > available_directories:
        raise PEFormatError("Data-directory count exceeds the optional-header bounds")
    if size_of_image == 0 or size_of_headers == 0 or size_of_headers > len(data):
        raise PEFormatError("Invalid SizeOfImage or SizeOfHeaders")
    if section_alignment == 0 or file_alignment == 0:
        raise PEFormatError("SectionAlignment and FileAlignment must be non-zero")

    data_directories: list[tuple[int, int]] = []
    for index in range(number_of_directories):
        rva, size = struct.unpack_from(
            "<II", optional, directory_table_offset + index * 8
        )
        data_directories.append((rva, size))

    section_table_offset = optional_offset + optional_size
    section_table_size = number_sections * SECTION_HEADER_SIZE
    _slice(data, section_table_offset, section_table_size, "section table")
    if section_table_offset + section_table_size > size_of_headers:
        raise PEFormatError("Section table extends beyond SizeOfHeaders")

    sections: list[dict[str, Any]] = []
    for index in range(number_sections):
        values = _unpack(
            "<8sIIIIIIHHI",
            data,
            section_table_offset + index * SECTION_HEADER_SIZE,
            "section header",
        )
        (
            name,
            virtual_size,
            virtual_address,
            raw_size,
            raw_pointer,
            _reloc_ptr,
            _line_ptr,
            _reloc_count,
            _line_count,
            section_flags,
        ) = values
        if raw_size and (
            raw_pointer > len(data) or raw_size > len(data) - raw_pointer
        ):
            raise PEFormatError(
                f"Section {_decode_name(name)!r} raw data is outside the file"
            )
        virtual_span = max(virtual_size, raw_size)
        if virtual_address + virtual_span > 0x100000000:
            raise PEFormatError(
                f"Section {_decode_name(name)!r} virtual range overflows"
            )
        sections.append(
            {
                "name": _decode_name(name),
                "virtual_size": virtual_size,
                "virtual_address": virtual_address,
                "raw_size": raw_size,
                "raw_pointer": raw_pointer,
                "characteristics": f"0x{section_flags:08X}",
            }
        )

    if entry_point:
        _rva_to_offset(entry_point, 1, sections, size_of_headers, len(data))

    import_rva, import_size = (
        data_directories[1] if len(data_directories) > 1 else (0, 0)
    )
    imports = _read_imports(data, import_rva, import_size, sections, size_of_headers)

    return {
        "format": format_name,
        "machine": machine_name,
        "machine_code": f"0x{machine:04X}",
        "initial_x86_x64_target": machine in (0x014C, 0x8664),
        "subsystem": SUBSYSTEM_NAMES.get(subsystem, f"unknown_{subsystem}"),
        "subsystem_code": subsystem,
        "is_dll": bool(characteristics & 0x2000),
        "entry_point_rva": f"0x{entry_point:08X}",
        "image_base": f"0x{image_base:X}" if image_base is not None else None,
        "section_alignment": section_alignment,
        "file_alignment": file_alignment,
        "size_of_image": size_of_image,
        "size_of_headers": size_of_headers,
        "timestamp": timestamp,
        "sections": sections,
        "imports": imports,
        "data_directories": [
            {"index": index, "rva": f"0x{rva:08X}", "size": size}
            for index, (rva, size) in enumerate(data_directories)
            if rva or size
        ],
        "execution_status": "metadata_only_not_executable_by_this_tool",
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Inspect Windows PE metadata without executing the file."
    )
    parser.add_argument("executable", type=Path, help="Path to a Windows .exe or .dll")
    parser.add_argument("--json", action="store_true", help="Emit machine-readable JSON")
    args = parser.parse_args(argv)

    try:
        size = args.executable.stat().st_size
        if size > 512 * 1024 * 1024:
            raise PEFormatError("File exceeds the 512 MiB inspection limit")
        data = args.executable.read_bytes()
        result = inspect_pe(data)
    except (OSError, PEFormatError) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2

    if args.json:
        print(json.dumps(result, indent=2))
    else:
        print(f"Format:      {result['format']}")
        print(f"Machine:     {result['machine']} ({result['machine_code']})")
        print(f"Subsystem:   {result['subsystem']}")
        print(f"Entry point: {result['entry_point_rva']}")
        print(f"Image base:  {result['image_base']}")
        print(f"Sections:    {len(result['sections'])}")
        print(f"Imports:     {', '.join(result['imports']) or '(none declared)'}")
        print("Execution:   metadata inspection only; this tool does not run the file")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
