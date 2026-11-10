# Reaper-Domain Integration — NUN

**Role:** specification scaffold for a minimal kernel substrate; no kernel implementation is currently present  
**Canonical catalogue:** [SAMBU — 50 curated engineering patterns](https://github.com/bboroski/SAMBU/blob/main/data/reaper/engineering_patterns_50.tsv)

## Architectural transfer

The Reaper material can be used as a resilience and systems-design case study. It suggests questions for the future kernel architecture:

- **Relay network / Citadel dependency:** how are service dependencies declared, admitted, monitored, and recovered?
- **Kinetic barrier / hull integrity:** where are address-space, privilege, memory, and fault-containment boundaries enforced?
- **Core reconstruction:** how does the supervisor restore a last-known-good capability snapshot after failed activation?
- **Synthetic-organic conflict:** how are native drivers, user services, and untrusted modules separated by ABI and permissions?
- **Harvest cycles:** how are boot, service lifecycle, update, rollback, and recovery represented as versioned state machines?

## Normative constraints

Use NUN's existing Capability Composition & Assimilation model: immutable artifact identity, manifest validation, deterministic dependency resolution, DAG checks, least privilege, staging, atomic activation, and rollback. The domain glossary itself is passive data and must never be executable input.

## Status and acceptance

NUN remains a specification repository until kernel source, linker scripts, build targets, and tests exist. Do not claim a bootable OS, physical Eezo-like subsystem, AI consciousness, or verified CPU support from this reference material.
