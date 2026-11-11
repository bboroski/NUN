# Engineering Governance — NUN

This document defines the repository-level change-control baseline. It does not claim that a control is enforced until the corresponding GitHub setting or required workflow check is enabled and verified.

## Pull request lifecycle

1. Keep work on a topic branch; never push directly to `main`.
2. Use **Squash and merge** as the default merge strategy. Keep the resulting commit title meaningful and conventional.
3. Rebase/update the topic branch against `main`; resolve conflicts and rerun all checks.
4. Require at least one independent approval (two for high-risk, security-sensitive, ABI, kernel, boot, release, or cross-repository changes).
5. Require all applicable CI checks and resolve every review conversation before merge.
6. Delete the head branch after merge.

## Required validation gates

Adapt commands to the actual languages and build systems in this repository; do not add placeholder commands or report checks that were not run.

- Formatting, linting, static analysis, and configuration/schema validation.
- Unit tests and integration tests, including boundary, failure-path, and regression cases.
- Build/package validation for every supported target or platform.
- Smoke/E2E tests for user-visible or deployable surfaces, where applicable.
- Dependency and secret scanning.
- Documentation/link checks when documentation changes.

Coverage is measured against the repository's established baseline. The initial target is **85% where meaningful and technically measurable**; do not invent coverage for generated, hardware-only, platform-unavailable, or specification-only code. Block regressions below the agreed per-project threshold and report both total and changed-line coverage.

## Merge and post-merge safety

- No force-push to `main`; protect the branch against deletion and non-fast-forward updates.
- Enable auto-merge only for low-risk dependency patch/minor updates after required checks and review policy are satisfied. Major updates and security-sensitive changes require human review.
- After merge, run the main-branch build and integration/smoke checks. Deploy only through an explicitly configured environment with approvals and health checks.
- Roll back with a reviewed revert or deployment rollback only after a regression is confirmed. Do not auto-revert on transient external-service failures or flaky tests; prevent rollback loops and preserve incident evidence.

## Documentation contract

- Update the README and relevant `docs/` pages in the same PR as behavior, interface, build, or deployment changes.
- Keep Quick Start reproducible and limited to three commands where the project can support that accurately.
- Keep badges tied to real workflows and published coverage/version sources.
- Maintain Mermaid architecture diagrams when they materially explain current data/control flow.
- Generate OpenAPI/GraphQL specifications only where those interfaces actually exist; do not imply an API where none is implemented.

## Dependency and security automation

Dependabot is configured for GitHub Actions updates. Add package-manager ecosystems only after confirming the repository's actual manifests and lockfiles. Review updates through isolated PRs. Enable repository/organization-level Dependabot alerts, secret scanning, and push protection where the plan and permissions support them.

## GitHub settings checklist (administrator action)

- [ ] Set `main` as the default branch.
- [ ] Require pull requests and the agreed number of approvals.
- [ ] Require CODEOWNERS review for owned paths, where CODEOWNERS exists.
- [ ] Require all applicable status checks; dismiss stale approvals or require approval of the latest push.
- [ ] Require conversation resolution.
- [ ] Block force pushes and branch deletion on `main`.
- [ ] Allow squash merging; disable unused merge methods if compatible with project policy.
- [ ] Enable automatic deletion of merged head branches.
- [ ] Enable Dependabot alerts, secret scanning, and push protection where available.

**Enforcement status:** checklist items are policy targets, not proof of active GitHub settings. Verify each setting in repository Rulesets/Branches settings and record the result.
