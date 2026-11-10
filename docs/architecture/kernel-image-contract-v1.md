# NUN Kernel Image Contract v1

**Capability ID:** kernel.image.contract.specification.v1  
**Status:** specification proposal; not implemented  
**Owner:** NUN  
**Consumer:** NUNLauncher

## Purpose

NUN owns the normative description of the kernel image and handoff data expected by its future bootstrap layer. NUNLauncher validates an image against this contract before transferring control. This document does not describe an implemented ELF loader, signed-image verifier, boot protocol, or bootable kernel.

## Required image metadata

A future versioned image header must declare:

- fixed magic and image-format version;
- target architecture identifier (x86-64, AArch64, RISC-V 64, or Elbrus/E2K);
- ABI family and version;
- image byte length and header length;
- entry-point offset or address;
- required load alignment;
- segment table location and bounded entry count;
- declared memory requirements;
- digest algorithm and digest location;
- optional signature metadata and key identifier.

Architecture and ABI identifiers are explicit. A portable uint64_t value is not evidence that the payload targets a 64-bit ISA.

## Segment contract

Every segment declaration must provide file offset, file size, memory size, load/virtual address, and permission flags. Validation must reject:

- integer overflow in offset or size calculations;
- file ranges outside the image;
- memory ranges outside the declared allowed map;
- overlapping segments unless an explicit format rule permits them;
- writable-and-executable segments unless the target policy explicitly permits them;
- invalid or unsupported permission bits;
- misaligned entry points and segment addresses;
- an entry point that is not inside an executable segment.

The loader must not execute payload code while validation is incomplete.

## Handoff contract

The NUNLauncher-to-NUN handoff must be a versioned structure containing the declared architecture and ABI, validated image range, entry point, memory-map descriptor, boot-source identity, and explicitly granted capabilities. Pointers and physical addresses must use target-defined widths and documented address spaces; they must not be assumed interchangeable.

NUN independently validates handoff version, structure size, alignment, range arithmetic, architecture compatibility, and reserved-memory boundaries before initializing kernel subsystems.

## Trust and failure policy

A digest detects accidental or deliberate changes only when its algorithm and trust context are defined. Authenticity requires a separately specified cryptographic signature and trusted-key policy. A checksum alone is not authentication.

Any unknown mandatory format feature, unsupported ABI, invalid range, failed digest/signature check, or malformed handoff must fail closed. No implicit architecture fallback is permitted.

## Acceptance gates

Before this capability can be marked implemented, the project needs a normative binary layout, schema or reference parser, test vectors, overflow and malformed-input tests, emulator coverage, and target-specific validation. Until then, this document is a planning contract only.
