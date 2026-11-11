# Engineering Pattern Integration — NUN

**Role:** specification scaffold for a minimal kernel substrate; no kernel implementation is currently present  
**Reference catalogue:** [SAMBU — 50 engineering patterns](https://github.com/bboroski/SAMBU/blob/main/data/reaper/engineering_patterns_50.tsv)

## Architectural mapping

The reference catalogue supplies questions for the future kernel contract:

- **Dependency admission:** how are service dependencies declared, admitted, monitored and recovered?
- **Protection boundaries:** where are address-space, privilege, memory and fault-containment checks enforced?
- **State recovery:** how does a supervisor restore a last-known-good capability snapshot after failed activation?
- **Interface isolation:** how are native drivers, user services and untrusted modules separated by ABI and permissions?
- **Lifecycle control:** how are boot, service activation, update, rollback and recovery represented as versioned state machines?

## Normative constraints

Use NUN's existing Capability Composition & Assimilation model: immutable artifact identity, manifest validation, deterministic dependency resolution, DAG checks, least privilege, staging, atomic activation and rollback. The catalogue is passive data and must never be executable input.

## Status and acceptance

NUN remains a specification repository until kernel source, linker scripts, build targets and tests exist. Do not claim a bootable OS, unimplemented physical subsystem, autonomous intelligence or verified CPU support from reference material.
