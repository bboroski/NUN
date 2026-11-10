# NUN Domain Capabilities

This specification converts Reaper and biology vocabulary into named kernel
capability contracts. It does not claim that the kernel implementation exists.

## Capability contracts

| Contract | Feature represented | Expected kernel boundary |
|---|---|---|
| `nun.resource-envelope.v1` | Bounded resource admission and release | Caller-owned allocator/resource manager |
| `nun.simulation-field.v1` | Mass-effect and quantum vocabulary as simulation parameters | Isolated, non-privileged simulation service |
| `nun.integrity-monitor.v1` | Integrity, anomaly and fault-containment observations | Read-only diagnostic monitor |
| `nun.lifecycle.v1` | Stage, verify, activate, rollback and retire cycles | Versioned supervisor state machine |
| `nun.consensus.v1` | Deterministic proposals, votes and quorum | Explicit IPC/network adapter |
| `nun.evolution-gate.v1` | Evidence-gated configuration evolution | Immutable artifacts plus rollback gate |

The machine-readable declarations are in `spec/domain-capability-profile.json`.

## Validation

```sh
python tools/validate_domain_profile.py
python -m unittest tests.test_domain_profile
```

## Status boundary

NUN remains specification-only. These contracts and their validator do not
create a kernel, scheduler, boot path, driver, hardware physics engine, biological
runtime, or verified architecture support. Implementations must separately
satisfy the existing capability-manifest, ABI, permission, memory and recovery
contracts before promotion.
