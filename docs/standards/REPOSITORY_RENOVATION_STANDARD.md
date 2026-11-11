# Repository Renovation Standard

> Shared engineering and presentation contract. Canonical repository names, product scope, architecture, and capability claims remain repository-specific.

## 1. Purpose and truthfulness
Align documentation, visual identity, GUI patterns, pull-request review, and technical-debt governance. This standard does not authorize renaming repositories, changing public APIs, or claiming unimplemented behavior. A weakness is not a feature until a user-visible capability, measurable service level, or verified mitigation exists. Describe limitations plainly; distinguish planned, experimental, supported, and deprecated states.

## 2. Visual identity

The canonical visual source of truth is `design/duck-ui.tokens.json` (schema 3.0.0), with the complete color specification in `design/DUCK_UI_SYSTEM.md`. It defines exactly six base colors and fourteen functional shades, each with HEX, RGB, HSL and a semantic purpose. Use those tokens; do not introduce a competing palette or copy older, incompatible values into new UI or documentation.

Black and graphite form the structural foundation. Yellow is reserved for primary actions, focus and selection. Red indicates errors/destructive actions, cyan information, gray neutral hierarchy and white readable content. Status must also use text, icon or shape; color alone is never sufficient. Check contrast in the actual foreground/background pairing.

## 3. Motion system

The authoritative catalogue of ten element animations and ten transition profiles—including keyframes, durations, easing, intended use and shared CSS utilities—is maintained in `design/DUCK_UI_SYSTEM.md` and `assets/duck-ui/duck-ui.css`. Do not duplicate or redefine timing values in this document.

Use motion to explain state changes, not as decoration. Prefer opacity and transform; respect `prefers-reduced-motion`; preserve the final state when motion is disabled. Persistent animation is limited to meaningful loading or live-state indicators. Native, CLI and specification-only projects must document the shared contract without claiming that graphical motion is implemented where no GUI exists.

## 4. GUI shell
Where a GUI exists, use a task-appropriate shell: header with product mark and truthful system status; main work area; grouped primary/secondary controls; footer/tabs only for multiple workspaces. Yellow canvas is optional and appropriate only when it helps the task. Freehand/vector/gesture input must only be advertised if implemented.
```text
+------------------------------------------------------------------+
| [MARK] Status: <measured state> | <metric> | <activity> | HH:MM  |
+------------------------------------------------------------------+
| Work area: task-specific content, grid/canvas, forms or results   |
| [Primary actions]                         [Context controls]     |
+------------------------------------------------------------------+
| [Current workspace]  o  o  o  (only when multiple pages exist)   |
+------------------------------------------------------------------+
```
Never hard-code sample metrics such as FPS or "55 ops". Every metric needs a source, unit, refresh policy, and unavailable state.

## 5. Weakness-to-feature governance
| Limitation | Honest framing | Required evidence |
|---|---|---|
| High latency | Async/batched processing, if implemented | Progress, cancellation, timing metrics, latency distribution |
| Unstructured handwriting | Freeform annotation, if implemented | Stroke/vector model, export, accessibility |
| No built-in GUI | Headless-first, if intentional | Stable CLI/API, examples, external GUI docs |
| Partial platform support | Scoped portability | Tested platform matrix and unsupported-state messaging |
| Technical debt | Transparent engineering roadmap | Tracked items, owners, risk, remediation status |

Never hide defects or rename missing functionality as a delivered feature. State actual limitation, mitigation, user impact, and evidence.

## 6. README contract
Every root README preserves the exact canonical repository name and documents actual purpose. Recommended sections: product mark/title; accurate summary/status; maintained language links; capabilities/architecture; verified quick start; real build/test/lint commands; GUI assets in `docs/gui/` when relevant; limitations/platform/security notes; contribution/release/support; maintained docs links. Do not invent commands, badges, coverage, screenshots, or capabilities. Archive obsolete docs only after checking inbound links and preserving historical value.

## 7. PR and Git governance
- Use short-lived feature/fix/docs branches; no direct commits to protected main/master/release.
- Require two independent approvals where team/plan supports it and branch protection enforces it. Do not claim enforcement until settings are verified.
- Merge only when all required CI checks pass; no bypass of failing/pending required checks.
- Keep PRs reviewable; link issues; document compatibility, migration, and rollback risks.
- Set coverage targets per language and baseline. 80% is a target only when meaningful; never falsify coverage or weaken tests to meet it.

## 8. RACI and documentation KPIs
| Activity | Responsible | Accountable | Consulted | Informed |
|---|---|---|---|---|
| Code/tests | Contributor | Maintainer | Reviewer | Team |
| README/API docs | Contributor/writer | Maintainer | API owner | Users |
| Design/accessibility | UI owner | Maintainer | Accessibility reviewer | Team |
| CI/security | CI owner | Maintainer | Security reviewer | Team |
| Release notes | Release owner | Maintainer | Contributors | Users |

Track README command validity, API-doc freshness, broken links, required-check pass rate, review latency, high-risk debt, accessibility findings, and platform coverage. Publish measured values with date/method; distinguish targets from results.

## 9. Voice and marketing
Be precise, direct, user-centered, and evidence-led. State audience, task solved, supported capabilities, and experimental boundaries. Marketing claims must trace to tests, benchmarks, screenshots, or release evidence. Position engineering investment as reliability, maintainability, transparency, and reduced risk—not as proof that defects are features.

## 10. Completion checklist
- [ ] README reflects current implementation and canonical name.
- [ ] Commands, links, badges, screenshots, and claims verified.
- [ ] Shared tokens and accessible contrast used where applicable.
- [ ] Motion improves interaction; reduced-motion honored.
- [ ] Limitations and mitigations are accurate.
- [ ] PR template and contribution guidance present.
- [ ] CI/tests/lint/docs checks pass or failures tracked.
- [ ] Branch protection and reviews verified, not merely documented.
- [ ] Obsolete docs archived only after link/history review.
- [ ] UA/EN/RU docs agree in meaning where maintained.
