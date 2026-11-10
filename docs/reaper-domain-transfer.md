# Reaper Domain Transfer Profile — NUN

**Status:** kernel capability and trust-boundary specification  
**Role:** versioned capability composition, isolation, resource limits and auditability

## Objective

Use the synthetic intelligence, distributed consensus, memory core, relay network and cyclic-process ideas from the source reference as architectural test cases for NUN's capability composition model. These concepts must become explicit contracts, not privileged “self-improving” kernel behavior.

## Safe system analogues

- **Synthetic agent:** an ordinary user-space or isolated service capability with a stable identity.
- **Collective consciousness / Geth consensus:** a distributed service whose members, protocol version and quorum policy are declared.
- **Memory core / archive:** bounded storage with provenance, integrity policy and explicit permissions.
- **Relay network:** a routed IPC graph with authenticated endpoints and resource budgets.
- **Harvest cycle:** a versioned maintenance or migration workflow with explicit start, completion and rollback state.
- **Indoctrination / neural override:** lore-only. Never use as a real user, medical or security classification.

## Capability requirements

Every capability remains bound to the existing manifest rules: stable ID, semantic version, ABI, provided/required interfaces, dependencies, permissions, artifact digest, provenance and resource limits. Composite capabilities must declare child IDs and conflict policy. They do not inherit the union of child permissions automatically.

Admission must remain staged:

`DISCOVERED → QUARANTINED → VALIDATED → STAGED → ACTIVE`

Failure, revocation or incompatible replacement must leave the previous active composition intact or move the capability to a disabled state. Dependency cycles are rejected. Identity is the tuple of ID, version and artifact digest, not the display name.

## Additional design constraints

- No dynamic kernel module execution until architecture-specific memory protection, ABI, recovery and trust mechanisms exist.
- No self-replicating or self-modifying capability is admitted by default.
- Distributed consensus does not establish truth; it establishes agreement under a declared protocol and threat model.
- A digest without authenticated provenance does not establish publisher identity.
- Permissions are least-privilege, scoped and reviewable. Resource limits are enforced by the relevant isolation boundary.
- Composition and activation produce auditable records with stable sequence and rollback references.

## Acceptance criteria

Tests must reject cyclic dependencies, conflicting IDs/digests, undeclared interfaces, permission escalation, missing artifact provenance, oversized resource requests and partial activation. A composite is visible as active only after all required validation gates succeed.

This document applies the Reaper vocabulary to a safe capability model. It does not implement Reaper AI, autonomous code assimilation, or a bootable NUN kernel.
