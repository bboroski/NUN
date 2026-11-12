# Engineering Improvement Index

> Repository-specific execution map for the shared engineering improvement standard. This is a control index, not a claim that every planned improvement is already implemented.

**Baseline review:** 2026-11-11  
**Default branch:** `main`  
**Project scope:** Early x86_64 kernel bring-up with separate boot paths, boot metadata contracts and QEMU validation.

## 1. Non-negotiable rules

- Preserve purpose, public behavior, APIs, data formats, supported platforms and development history unless a separately reviewed migration explicitly changes them.
- Treat this repository's source, manifests, tests and release process as authoritative. Do not impose another project's architecture, language, UI or deployment model.
- Prefer small, isolated and reversible changes. Each PR has one coherent objective, acceptance criteria, affected components, risk statement and rollback path.
- Do not delete unknown files, disable existing checks, substitute mocks for real tests, or describe a specification, scaffold, mock-up or open PR as a shipped feature.
- Use truthful states: **not assessed**, **planned**, **implemented**, **verified**, **experimental**, **blocked** and **unsupported**. A configured workflow is not a passed workflow.
- Never commit secrets, tokens, personal data, machine-specific paths or unreviewed binaries.

## 2. Repository evidence map

Product scope and supported behavior: [README](../README.md). Paths below were discovered during structural inventory; their presence does not imply a successful runtime check.

### CI and automation
- [ci.yml](../.github/workflows/ci.yml)
- [boot.yml](../.github/workflows/boot.yml)
- [host-contract-tests.yml](../.github/workflows/host-contract-tests.yml)
- [qemu-kernel.yml](../.github/workflows/qemu-kernel.yml)

### Tests and validation sources
- [tests/](../tests/)

### Existing governance and project rules
- [ENGINEERING_GOVERNANCE.md](./ENGINEERING_GOVERNANCE.md)
- [TECHNICAL_DEBT_REGISTER.md](./TECHNICAL_DEBT_REGISTER.md)
- [standards/REPOSITORY_RENOVATION_STANDARD.md](./standards/REPOSITORY_RENOVATION_STANDARD.md)

- Pull request checklist: [repository template](../.github/pull_request_template.md).

**Verification boundary:** this inventory did not execute the build, tests, target hardware, security scanner or release process. Use results attached to each PR's exact head commit as execution evidence. Missing or skipped checks remain unverified.

## 3. Required engineering workstreams

### Architecture and code
Map entry points, modules, ownership, data flow and external boundaries before refactoring. Identify cycles, duplicated logic, dead code, uncontrolled global state and unnecessary abstractions. Keep interfaces narrow and errors explicit. Refactor only with behavior-preserving tests or a documented migration.

### Application and user experience
Audit actual user journeys, command/API flows and GUI surfaces where they exist. Check loading, empty, error, disabled and recovery states; keyboard access, focus, contrast, responsive layout and reduced motion where applicable. A kernel, loader or headless service must not be forced to grow a GUI to satisfy a generic checklist.

### Testing and compatibility
Use meaningful unit, integration, functional, regression, smoke, emulator and target-specific tests as appropriate. Cover normal, boundary and failure paths. Preserve supported behavior and formats; document unsupported platforms and hardware qualification. Host CI does not prove target-hardware behavior.

### Build and CI/CD
Keep builds reproducible and workflows least-privileged. Run native formatting, lint, static analysis, tests, builds, documentation and security checks where configured. A check must fail on a real defect and produce actionable output; do not add permanently green placeholder jobs.

### Security and dependencies
Review input validation, authorization, secret handling, file/path access, logging, network boundaries, workflow permissions, dependency vulnerabilities and license compatibility. Update dependencies in small, compatible groups and retain reproducibility controls. Record severity, exposure and remediation evidence.

### API, configuration and data
Treat public APIs, schemas, configuration keys, persistence formats, boot metadata, handoff structures and runtime contracts as versioned interfaces. Validate inputs and outputs, define error behavior and timeouts, version incompatible changes, and provide tested migrations and recovery for destructive operations.

### Performance and operations
Measure startup, latency, memory, CPU, I/O and artifact size before optimizing. Add diagnostics and metrics only where they improve fault isolation. Do not log secrets or add cache/parallel/distributed complexity without evidence.

### Documentation and visual assets
Keep root `README.md` under 5,000 characters, focused on purpose, current capabilities, real setup/test commands, limitations, useful documentation and a small number of relevant cross-links. Use current, repository-owned screenshots of the actual application only. If there is no GUI, omit screenshots or clearly label a genuine architecture diagram; never present a mock-up as a screenshot. End the root README with a correctly translated Chinese love quotation. Keep maintained language variants aligned.

## 4. Change delivery and integration

1. Record the current `main` SHA, repository state and baseline check results before code changes.
2. Create a short-lived branch from current `main`; do not work directly on a protected branch.
3. Split architecture, application, security, testing, CI, dependency and documentation changes into independent PRs when reviewable separately.
4. Include exact commands, workflow links, affected platforms, compatibility notes, known limitations, evidence and rollback steps.
5. Review the full diff; reject unexplained deletion, broad formatting churn and unrelated dependency changes.
6. Merge only after applicable checks pass on the current PR head, blocking review threads are resolved and required approvals/rules are satisfied. Where merge order matters, integrate sequentially and rerun checks after each merge.
7. Verify the resulting `main` commit and retain PR, commit and test evidence. Never bypass protections or treat pending/skipped checks as success.

## 5. Technical-debt governance

Record each confirmed issue with a stable ID, subsystem, evidence, user/operational impact, severity, likelihood, owner/role, mitigation, proposed action, target milestone and status. Prioritize critical security, data-loss and service-blocking failures before cosmetic cleanup. Accepted debt needs a reason, compensating control and review date. An empty register means **not yet recorded**, not **no technical debt**.

## 6. Definition of Done

- Scope and acceptance criteria are explicit; the change is focused and reversible.
- Relevant tests exercise success, boundary and failure behavior.
- Applicable build, lint, static, security and documentation checks have real results.
- API, configuration, data and platform compatibility have been reviewed.
- README and guides match shipped behavior; screenshots and claims are genuine.
- Known limitations and remaining debt are documented without overstating completion.
- The PR is independently reviewable and the target branch remains buildable.

**Current status:** repository structure and existing controls have been mapped. Complete architecture, runtime security, performance, hardware qualification and end-to-end behavior still require repository-specific execution and review.
