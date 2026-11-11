# Technical Debt Register — NUN

**Review scope:** initial review of the Duck System 3.0 pull request, changed documentation/design assets and PR-triggered GitHub Actions visible on 2026-11-11.  
**Audit status:** partial baseline; this is not a comprehensive architecture, security, dependency, performance or accessibility audit.  
**Rule:** absence of a recorded item does not mean absence of technical debt.

## Findings

| ID | Domain | Impact | Likelihood | Risk | Effort (1–5) | Owner | Target | Acceptance decision | Status | Evidence | Next action |
|---|---|---:|---:|---:|---:|---|---|---|---|---|---|
| NUN-CI-001 | Runtime / memory accounting | 5 | 5 | Critical (25) | 3 | TBD (maintainer) | Before next release | Pending triage | identified | `NUN integration CI` fails to compile `src/runtime/runtime.c`: `nun_runtime_t` has no member `arena_reserved_bytes`, referenced during rollback, initialization, allocation and arena reset. | Inspect the runtime structure and arena ownership/lifecycle contract; reconcile accounting with overflow and rollback behavior; add initialization, allocation, reset and failure-path tests. |

## Risk method

Score impact and likelihood independently from 1 to 5; risk = impact × likelihood. Bands: 1–4 Low, 5–9 Moderate, 10–16 High, 17–25 Critical. Scores are preliminary triage, not a substitute for owner review. Re-score after mitigation and when exposure changes.

## Required record fields

Every new item must include a stable ID, subsystem, reproducible evidence, impact, likelihood, risk, remediation estimate, owner, mitigation, target milestone, acceptance decision and status. Allowed statuses: `identified`, `triaged`, `planned`, `in_progress`, `mitigated`, `resolved`, `accepted`.

Accepted debt requires a documented reason, owner, mitigation and review/expiry date. Security-critical debt cannot be accepted solely to meet a release date. Close an item only after the fix is merged and verification evidence is linked.

## Next audit

Complete the repository-wide baseline before the next major release and refresh at least quarterly. Cover architecture, security/privacy, dependencies/supply chain, testing, error handling, resource use, accessibility, documentation drift, build/release automation and public-contract compatibility. Mark unreviewed areas **not assessed**; do not populate speculative findings.
