# NUN Boot Chain and Image Contract

**Status:** specification; no EFI loader or bootable image is implemented yet.  
**Initial profile:** x86_64 UEFI under QEMU/OVMF.

## 1. Components and ownership

| Component | Owns | Does not own |
|---|---|---|
| UEFI firmware | CPU/platform initialization, firmware memory map, boot-device discovery | NUN process and kernel policy |
| NUN EFI loader (planned) | Image verification, memory allocation before handoff, initrd loading, boot-info construction | Kernel page allocator, scheduler and device drivers |
| NUN kernel | Revalidation, memory ownership after handoff, architecture setup, services | Firmware boot services after ExitBootServices |
| NUNLauncher Android app | Android Home experience and visible Android app catalog | EFI execution, fastboot, partition writes or kernel handoff |

NUNLauncher is not the NUN EFI loader. The EFI loader is a separate boot artifact in the NUN boot chain.

## 2. Image container

The first image format must be deliberately small and inspectable. A proposed v1 container has:

- magic and format version;
- target architecture and minimum CPU feature mask;
- kernel payload offset and length;
- initrd offset and length;
- boot-info ABI major/minor;
- SHA-256 digest for each payload;
- signature algorithm, key identifier and signature block;
- build identifier, source revision and reproducible-build metadata;
- optional anti-rollback version.

All offsets and lengths are checked with overflow-safe arithmetic before a payload is read or mapped. Unknown mandatory flags, overlapping payload ranges, unsupported architectures, malformed signatures and digest mismatches are fatal. No executable payload is run during parsing.

The image container schema must be versioned independently from the kernel's boot-info ABI. A loader and kernel may support different container and boot-info minor versions only where compatibility rules explicitly permit it.

## 3. UEFI loader algorithm

1. Parse the container using bounded reads.
2. Verify target architecture and required CPU features.
3. Verify payload bounds, digest, signature and rollback policy.
4. Allocate destination memory using firmware services and retain all allocation metadata.
5. Load kernel and initrd without assuming a fixed physical address.
6. Prepare architecture-specific page tables or a documented transition environment.
7. Build the boot-info record and memory-region list.
8. Acquire the final UEFI memory map and current map key.
9. Call `ExitBootServices()` with that map key.
10. On a stale map key, reacquire the map and retry according to UEFI rules; do not call unrelated boot services after a partially successful first attempt.
11. Transfer control through the architecture entry ABI.

A production loader also needs bounded retry behavior, explicit diagnostics and a recovery route if verification or handoff fails.

## 4. Boot-information v1

The C layout in `include/nun/boot_info.h` is the host-testable source for the first v1 ABI. The contract includes:

- magic constant and ABI revision;
- declared structure size;
- architecture identifier;
- flags reserved as zero in v1;
- physical location of the memory map, count and exact entry size;
- kernel physical base and byte length;
- initrd physical base and byte length;
- ACPI RSDP and device-tree physical addresses when present;
- framebuffer physical base and geometry when present.

Memory-map entries contain physical base, byte length, region type and attributes. Entries must be non-empty, non-overlapping, ordered by increasing base and representable without unsigned overflow. The initial implementation imposes a fixed maximum entry count so early boot does not require dynamic allocation.

The kernel must map the memory-map storage before passing a mapped pointer to `nun_boot_info_validate()`. The validator checks the contents it is given; it cannot prove that an arbitrary physical address is safely mapped.

## 5. Ownership transfer

Before handoff, the loader owns its allocations and can use UEFI boot services. After successful `ExitBootServices()`, NUN owns the memory ranges explicitly transferred by the boot contract. The first allocator must exclude every non-usable region, including the kernel image, initrd, page tables, boot-info record, framebuffer, MMIO, reserved firmware and ACPI NVS regions.

No service may reuse the loader stack or boot-info storage until NUN has copied or permanently reserved it.

## 6. Validation checklist

- [ ] Container header and version checks.
- [ ] Overflow-safe offset and length validation.
- [ ] Signature and digest checks before execution.
- [ ] Architecture and CPU-feature gate.
- [ ] Kernel/initrd overlap and alignment checks.
- [ ] Memory-map ordering, overlap, count and overflow checks.
- [ ] Final map-key acquisition and ExitBootServices retry handling.
- [ ] Boot-info v1 validation in the kernel.
- [ ] Serial failure diagnostics before handoff.
- [ ] QEMU/OVMF boot smoke test and fault-injection matrix.
- [ ] Reproducible image digest and release provenance.

Passing the host-side boot-info tests proves only that validator's input checks. It does not prove the loader algorithm or a valid boot image.
