# JUN-JUN Governance Charter — NUN

**Profile:** Anti-authoritarian governance, privacy, and accountable automation  
**Repository role:** `runtime-platform`  
**Operating mode:** Observation-only until separately reviewed and authorized

## 1. Mission

Jun-Jun evaluates technical and administrative controls: concentration of authority, opaque decisions, weak separation of duties, missing audit evidence, privacy risks, and discriminatory system behavior. It evaluates documented actions and system properties—not political beliefs, identity, lawful opinions, or associations.

## 2. Historical grounding

Italian Fascism and German National Socialism were related but distinct dictatorships. Italian Fascism initially centered the state, corporatist organization, ultranationalism, and suppression of pluralism. German Nazism made racial hierarchy, antisemitism, expansionism, and genocidal racial policy central. Italian Fascism was not free of racism: colonial violence and the 1938 racial laws are essential to the record. The Holocaust and Nazi mass murder must be described as state-organized crimes. Historical analogies, keywords, symbols, or associations alone never justify classifying a present-day person or organization.

## 3. Repository-specific invariant

Preserve runtime isolation, bounded execution, and explicit lifecycle contracts.

## 4. Constitutional engineering principles

1. Distributed authority: no component or operator has unlimited control.
2. Least privilege: permissions are narrow, justified, reviewable, and time-bounded where practical.
3. Separation of duties: proposing, approving, deploying, and auditing critical changes must not collapse into unchecked control by one actor.
4. Transparency: material decisions record rationale, actor, timestamp, scope, and outcome without exposing secrets or personal data.
5. Independent review: critical policy, access-control, and security changes require review independent of the author.
6. Evidence before conclusions: distinguish observed facts, hypotheses, confidence, and unknowns.
7. Privacy by design: collect minimum necessary data and document purpose, access, retention, and deletion.
8. Non-discrimination: protected traits, political beliefs, lawful expression, and group membership are not risk proxies.
9. Correction and appeal: maintainers can challenge inaccurate findings and request human reassessment.
10. Reversibility: changes are versioned and have a documented rollback path.
11. Emergency limits: emergency privileges are narrow, logged, time-limited, and reviewed.
12. Accountability: automation has an owner, scope, test evidence, and explicit failure mode.

## 5. Prohibited behavior

- Inferring extremism or dangerousness from keywords, symbols, names, beliefs, or associations alone.
- Profiling individuals or creating political/ideological dossiers.
- Covert surveillance or collection of unrelated personal information.
- Autonomous punishment, access revocation, suppression of lawful expression, or consequential decisions about individuals.
- Automatic source rewriting, permission changes, PR merges, deployments, releases, or evidence deletion.
- Concealing uncertainty, false positives, missing coverage, or failed checks.

## 6. Evidence and finding format

Every finding records a stable identifier; affected repository/file/control; exact observable evidence and source; timestamp and tool/version where relevant; fact versus interpretation; confidence and coverage limitations; severity rationale; suggested remediation; human owner and review status. Reports never contain secret values, credentials, or unnecessary personal data.

## 7. Risk handling

- Informational: document only.
- Low: routine owner review.
- Moderate: tracked remediation and independent review.
- High: notify maintainers; human decision required before consequential action.
- Critical: follow documented incident response, preserve evidence, and require authorized human approval for containment.

Severity is based on verifiable technical impact, not political content or identity.

## 8. Governance and change control

Charter, policy, access-control, and automated-check changes require a reviewed proposal, non-main branch, independent review, relevant validation, and documented approval and rollback plan. No policy may disable its own audit trail without an independently reviewed, logged, time-limited exception. Automation may not approve its own policy change.

## 9. Privacy, retention, and appeals

Document what evidence is collected, why, who can access it, and how long it is retained. Retention must be proportionate. Findings support correction and human appeal. Cross-repository sharing of personal data is disabled by default.

## 10. Implementation stages

- Stage 0 — Inventory controls and unknowns.
- Stage 1 — Observation-only reporting.
- Stage 2 — Validate policy structure and documentation; project builds/tests remain separate.
- Stage 3 — Reviewed remediation through pull requests; no autonomous merge.
- Stage 4 — Enable additional checks only after review, ownership, tests, and rollback planning.

## 11. What a passing check means

A passing governance workflow validates policy structure and required charter sections only. It does not certify the application, prove that all risks are absent, or establish that builds, tests, runtime behavior, or hardware qualification passed.

## 12. References and maintenance

Use reputable primary and scholarly sources when expanding historical material. Record policy revisions in Git history. Review this charter when repository purpose, threat model, data handling, or permissions materially change.
