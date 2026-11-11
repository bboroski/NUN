## Summary
<!-- What changed and why? -->

## Scope and compatibility
- [ ] Scope is limited to the stated change.
- [ ] Public APIs, ABI, file formats, CLI behavior, and repository identity remain compatible or migrations are documented.
- [ ] Cross-repository dependencies and pinned versions were checked.

## Validation evidence
- [ ] Formatting / lint / static analysis: `<command + result>`
- [ ] Unit tests: `<command + result>`
- [ ] Integration / smoke / E2E tests (if applicable): `<command + result>`
- [ ] Build and supported-platform matrix: `<command + result>`
- [ ] Coverage: baseline `<x>`, result `<y>`, changed lines `<z>`
- [ ] Security / dependency / secret checks: `<command + result>`

Do not mark a check complete unless it was actually executed. Explain unavailable hardware, OS, credentials, or external services.

## Documentation
- [ ] README and relevant docs updated.
- [ ] Quick Start commands verified.
- [ ] API/schema/architecture diagrams updated where applicable.
- [ ] Claims describe implemented behavior only.

## Risk and recovery
- Risk: <low / medium / high>
- Failure modes:
- Rollback / recovery plan:
- Data or migration impact:

## Review checklist
- [ ] No secrets, credentials, or private data committed.
- [ ] No direct changes to `main`.
- [ ] All review conversations resolved.
- [ ] Required CI checks are green.
