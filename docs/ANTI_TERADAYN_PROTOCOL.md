# Anti-Teradayn Protocol — NUN

**Status:** repository-level engineering standard. All implementation and readiness claims require source, tests, and target evidence.  
**Scope:** this repository only. The protocol does not authorize destructive rewrites, blanket dependency removal, or implicit execution of code from another repository.

## Purpose

Apply measurable engineering discipline: bounded responsibilities, explicit interfaces, predictable resource use, minimal necessary dependencies, and reproducible verification. “Typified steaming” means typed, bounded, reviewable changes—not a new runtime or a claim of zero entropy.

## Seven-phase protocol

1. **Dependency discipline.** Inventory dependencies. Remove only components proven unnecessary and only after a tested replacement exists. Keep web/GUI/scripting/network layers that form part of the product; isolate their trust boundaries.
2. **Corpuscular transparency.** Prefer explicit data flow, narrow functions, fixed-width types at ABI boundaries, clear ownership, and measured hot paths. Use assembly only for demonstrated platform needs. Swift is optional, never a universal bridge/compiler layer.
3. **Driver and hardware boundaries.** Put privileged access behind native platform adapters with explicit contracts, error paths, and capability checks. Ruby is not a kernel-driver ABI. Keep browser code out of privileged execution paths.
4. **Applicable validation matrix.** Run tests that match this product: unit, integration, static, build, parser/schema, emulator, and hardware checks as applicable. GUI, Lua, rendering, interrupt, and physical-device tests are conditional. Label host/emulator/target evidence separately.
5. **Hardened integration.** Use least privilege, validate untrusted inputs, bound timeouts and resource use, and document cache/retention. Google services and external APIs are optional; do not add dynamic dependencies to static/offline paths without need.
6. **SAMBU relationship.** Use SAMBU only for real statistical/scientific workloads through versioned contracts. Do not embed modeling utilities in a portal, launcher, or kernel merely to satisfy a slogan.
7. **Resource allocation.** Keep core compute in the native implementation and UI/orchestration in supported existing layers. C/C++/C#/Swift/Ruby are not mandatory tracks. “Six-molecule allocation” is not a technical rule; define memory, CPU, concurrency, and latency budgets in measurable units.

## Typed-steaming rules

- **Never boil:** no broad refactor without scoped behavior change and regression coverage.
- **Never over-boil:** keep modules bounded; prefer composition and explicit ownership.
- **Never cook:** avoid hidden global state, unbounded queues, implicit network calls, and unnecessary runtime work.
- Preserve public contracts unless a versioned migration is supplied.
- No performance claim without reproducible benchmarks.
- No production, OS, driver, or hardware-readiness claim from mockups, specifications, or host tests alone.

## Repository-specific application

**Verified profile:** early x86_64 minimal-kernel substrate with boot contracts, CPU entry/privilege/exception/interrupt architecture, memory and execution models, and a separate Limine/QEMU bring-up path. The README explicitly states this is not a complete OS. Keep assembly and C tied to the actual x86_64 ABI; do not claim cross-ISA or Elbrus support without a distinct port and target evidence. Validate reproducible builds, boot handoff, memory-map invariants, exception paths, and emulator logs separately from physical hardware qualification. Suggested target checks: `bash scripts/build-qemu.sh`; `bash scripts/run-qemu.sh`.

## Acceptance checklist

- [ ] Scope and intended behavior are explicit.
- [ ] Dependency/security impact is recorded; no blanket technology purge.
- [ ] Interfaces, ownership, trust boundaries, error paths, and resource limits are documented.
- [ ] Relevant tests/checks pass; unsupported target checks remain pending.
- [ ] Documentation matches the implementation and accurately labels maturity.
- [ ] Cross-repository integrations use versioned contracts and never execute foreign code implicitly.

## Ecosystem boundary

Repositories remain independently buildable with their own APIs, release cycles, and evidence. Federation means documented contracts, not a forced monolith or shared runtime. See the [Hellduck Integration Playbook](https://github.com/bboroski/Hellduck/blob/main/docs/INTEGRATION_PLAYBOOK.md) where applicable.

---

*Make every claim testable, every boundary explicit, and every optimization measurable.*
