# Federated reliability model

NUN is the consumer-side kernel project. The repository currently contains specifications and host-side validators; it does not yet contain a kernel, bootable image or executable loader.

## Inbound contract

- NUN consumes a versioned `nun.boot-handoff.v1` record.
- The host-side validator rejects duplicate keys, unknown fields, malformed image metadata, range overflow, overlapping memory regions and entry points outside the declared kernel region.
- Capability identifiers are explicit and unique; unsupported fields are rejected rather than silently ignored.
- The canonical record digest detects content changes only when compared with a separately trusted value.

## Required kernel-side gates

A future kernel must independently validate the live firmware handoff, architecture and privilege state, physical memory map, executable image mapping and entry conditions. Metadata validation does not prove that a physical address is safe or that an image is authentic.

## Current scope

```bash
python tools/validate_boot_handoff.py --self-test
python tools/validate_boot_handoff.py spec/boot-handoff.example.json
```

These commands test contract metadata only. They do not compile a kernel or prove bootability, driver support or architecture compatibility.
