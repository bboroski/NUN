# Boot handoff consumer contract

NUN treats boot metadata as untrusted input until every field passes validation.
The shared `nun.boot-handoff.v1` contract is mirrored in this repository and in
NUNLauncher. The kernel implementation does not yet exist; this host-side tool
is an executable contract check, not a kernel entry path.

## Consumer invariants

- Reject unknown fields, duplicate JSON keys, unsupported schema versions and oversized records.
- Validate the image reference, declared image size and SHA-256 format.
- Validate unsigned 64-bit memory ranges, detect overflow and reject overlap.
- Require exactly one kernel region and an entry point within that region.
- Accept only unique, explicit capability identifiers.
- Verify the canonical record digest before interpreting the handoff.

## Run

```bash
python tools/validate_boot_handoff.py --self-test
python tools/validate_boot_handoff.py spec/boot-handoff.example.json
```

The example uses an illustrative architecture label and fabricated image digest;
it does not establish architecture support. SHA-256 detects content mismatch but
does not authenticate the producer or authorize execution. A future NUN kernel
must independently validate the live boot context, physical mappings, privilege
state, and architecture-specific entry conditions. No kernel binary or bootable
image is produced by this validator.
