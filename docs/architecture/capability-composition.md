# NUN Capability Composition & Assimilation

**Status:** architecture specification  
**Implementation status:** not implemented  
**Scope:** kernel-adjacent capability services and the future NUN system supervisor

## 1. Purpose

NUN must be able to admit new capabilities, understand what they provide and require, combine compatible capabilities, and expose the resulting composition as a reusable capability.

The mechanism is called **Capability Composition & Assimilation (CCA)**. It is a controlled composition system, not an unrestricted binary merger or permission-escalation mechanism.

A capability may be a service, driver, library, language toolchain component, adapter, or another composite capability. A composite is itself addressable, versioned, inspectable, and eligible for further composition.

## 2. Core model

Each admitted capability is represented by an immutable artifact and a manifest.

The manifest declares:

- stable reverse-domain identifier and semantic version;
- capability kind and target ABI family/version;
- provided interfaces and required interfaces;
- explicit dependencies and version constraints;
- requested permissions with resource scopes;
- artifact format and cryptographic digest;
- provenance metadata, publisher, license, and optional signature;
- for a composite, its child capability identifiers and conflict policy.

The machine-readable baseline is defined in [capability-manifest.schema.json](../../spec/capability-manifest.schema.json).

### 2.1 Capability identity

A capability is identified by `id + version + artifact digest`, not by its display name or file path. Renaming a module must not silently change its identity. Two artifacts with the same identifier and version but different digests are distinct candidates and must trigger an explicit conflict.

### 2.2 Interfaces before implementation details

Composition occurs against declared interfaces and ABI contracts. The resolver must not assume that two modules are compatible because they use the same language, file format, or function names.

An adapter may bridge incompatible interfaces only when an explicit adapter capability declares both sides and passes validation. The system must not invent or execute arbitrary glue code during admission.

## 3. Capability lifecycle

The registry exposes the following lifecycle states:

1. **discovered** — metadata is visible; no artifact code is trusted or executed.
2. **quarantined** — artifact and provenance are recorded; execution is prohibited.
3. **validated** — manifest schema, digest, signature policy, ABI, dependencies, and declared permissions pass policy checks.
4. **resolved** — dependency versions and interface providers are selected; conflicts are either resolved by explicit policy or rejected.
5. **staged** — the proposed composition is assembled in an isolated namespace with resource limits.
6. **active** — a complete registry snapshot is atomically published.
7. **disabled** — the capability remains recorded but is not exposed to consumers.
8. **revoked** — the capability is blocked from future activation; provenance remains auditable.

A state transition must be explicit and logged. Failed admission returns the candidate to quarantine or marks it rejected; it must never partially publish a composite.

## 4. Admission and composition pipeline

The normative pipeline is:

1. **Discover and ingest.** Accept a local package, trusted repository artifact, or built-in module. Record source, publisher, license, timestamp, and digest. Do not execute it.
2. **Validate.** Parse the manifest against the JSON Schema; verify the digest; apply signature and trust policy; check ABI family and compatibility.
3. **Resolve.** Select dependency versions deterministically. Require every declared interface to have a compatible provider. Reject missing, ambiguous, or cyclic dependencies.
4. **Plan composition.** Build a directed acyclic dependency graph (DAG). Preserve stable capability IDs and explicit edges. Produce a plan before changing the active registry.
5. **Resolve conflicts.** Apply only a caller-supplied policy: reject, namespace, use a declared adapter, or select an explicitly named preferred provider. There is no implicit last-writer-wins behavior.
6. **Stage.** Instantiate the graph in an isolated namespace with resource quotas and capability-scoped handles. Run interface, ABI, and health checks.
7. **Commit.** Publish the new registry snapshot atomically. Keep the previous snapshot available until the new snapshot passes its activation checks.
8. **Observe or rollback.** Monitor declared health signals. On failed activation or a failed invariant, atomically restore the last known-good snapshot and quarantine the failed candidate.

No step may silently widen the permissions of an existing capability.

## 5. Composition semantics

The composition operation is conceptually:

```text
compose(
  inputs: CapabilityRef[],
  policy: CompositionPolicy,
  target_abi: AbiTarget
) -> CompositionPlan | CompositionError
```

The result contains:

- a deterministic composition identifier and version proposal;
- the selected child capability references and dependency graph;
- resolved interface bindings and explicit adapter references;
- the effective permission set;
- resource limits and isolation boundaries;
- a provenance record linking every output to its inputs;
- validation steps and an activation/rollback plan.

### 5.1 Reusable and recursive composition

A composite may be an input to another composition. This permits capabilities to be combined in layers and lets NUN reuse previously validated combinations.

The dependency graph must remain acyclic. Nested composition is allowed; dependency cycles are not. Implementations must impose a configurable maximum graph depth and node count to prevent unbounded expansion.

### 5.2 Determinism and identity

Given the same immutable inputs, target ABI, resolver version, and explicit policy, planning must produce the same normalized graph and digest.

Repeated ingestion of the same artifact digest is idempotent. Repeated composition must not duplicate a child capability unless the policy explicitly declares separate isolated instances.

Composition is not assumed to be commutative: order may matter when an explicit provider preference or adapter chain is part of the policy. Any such ordering must be recorded in the plan.

### 5.3 Conflict policy

Supported baseline policies:

- `reject` — stop when providers, identifiers, or resource claims conflict;
- `namespace` — preserve both capabilities under distinct namespaces if their interfaces permit coexistence;
- `adapter` — use only a separately declared and validated adapter;
- `prefer-explicit` — select a provider named by the composition request.

An unspecified conflict is an error, never an implicit overwrite.

## 6. Security and fault containment

CCA is a privileged system boundary. The implementation must enforce:

- **No execution during discovery or validation.**
- **Least privilege:** permissions are explicit, scoped, and granted by policy. A composite does not automatically inherit the union of every child's privileges.
- **Isolation:** staged modules run in separate namespaces or processes, with resource quotas and mediated IPC.
- **Provenance:** immutable digests and source records are retained for every child and composite.
- **Trust policy:** signatures may be required by deployment policy; a valid signature does not replace ABI, dependency, or behavior checks.
- **Revocation:** a revoked artifact cannot be silently reintroduced through a composite.
- **Atomicity:** incomplete compositions are never visible as active capabilities.
- **Auditability:** admission, resolution, activation, disablement, and rollback produce structured diagnostic records.

Native kernel extensions must not be dynamically loaded until the relevant architecture, memory protection, ABI, and recovery mechanisms exist. A safer initial implementation should compose declarative services or isolated user-space modules before considering privileged native code.

## 7. Public service boundaries

The future NUN supervisor should expose versioned operations equivalent to:

| Operation | Responsibility |
|---|---|
| `capability.discover` | Enumerate known manifests and sources without executing artifacts |
| `capability.ingest` | Record an artifact in quarantine |
| `capability.validate` | Validate manifest, digest, trust, ABI, and permissions |
| `capability.resolve` | Resolve dependencies and interface providers |
| `capability.compose` | Produce a deterministic composition plan |
| `capability.stage` | Build and test an isolated candidate snapshot |
| `capability.activate` | Atomically publish a validated snapshot |
| `capability.rollback` | Restore a prior known-good snapshot |
| `capability.inspect` | Explain graph, interfaces, permissions, provenance, and state |
| `capability.detach` | Disable a capability after dependent-graph checks |

These are planned service contracts, not implemented API endpoints.

## 8. Failure model

The resolver and supervisor must return typed failures rather than partially succeeding. Minimum error classes:

- invalid or unsupported manifest schema;
- digest mismatch or failed signature policy;
- unsupported ABI or interface version;
- missing, ambiguous, or cyclic dependency;
- identifier, interface, or resource conflict;
- denied permission or exceeded quota;
- adapter unavailable or incompatible;
- staging health check failure;
- activation failure or rollback failure.

A rollback failure is a critical system fault and must be surfaced through the kernel diagnostic path.

## 9. Verification plan

Before CCA can be considered implemented, tests must cover:

- manifest parsing, schema rejection, and unknown-field handling;
- digest mismatch and signature-policy outcomes;
- deterministic dependency resolution and reproducible plans;
- missing providers, ambiguous providers, and dependency cycles;
- nested composites, repeated ingestion, and idempotency;
- every conflict policy, including the default rejection path;
- permission non-escalation and resource quota enforcement;
- staged isolation and IPC boundary enforcement;
- atomic activation, failure injection, and rollback;
- revocation propagation through nested composites;
- graph depth/node limits and malformed-input fuzzing.

Host-side tests are necessary but do not replace emulator, boot, and target-platform tests.

## 10. Delivery sequence

1. **Manifest and registry:** schema validation, stable identifiers, artifact digests, and quarantine state.
2. **Resolver:** interface matching, dependency constraints, ABI checks, and deterministic graph planning.
3. **Composition planner:** nested composites, explicit conflict policies, and provenance records.
4. **Staging supervisor:** isolated execution, quotas, health checks, and structured diagnostics.
5. **Transactional activation:** atomic snapshots, rollback, disablement, and revocation.
6. **User-facing control plane:** inspect, ingest, compose, activate, and rollback commands/UI with a single guided workflow.

## 11. Current repository status

This document and its schema define the intended architecture only. NUN currently has no kernel source, capability registry, resolver, supervisor, runtime, build system, or automated tests. No capability composition should be described as operational until those components are implemented and verified.
