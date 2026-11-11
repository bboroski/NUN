# Repository Renovation Standard

> Shared engineering and presentation contract. Canonical repository names, product scope, architecture, and capability claims remain repository-specific.

## 1. Purpose and truthfulness
Align documentation, visual identity, GUI patterns, pull-request review, and technical-debt governance. This standard does not authorize renaming repositories, changing public APIs, or claiming unimplemented behavior. A weakness is not a feature until a user-visible capability, measurable service level, or verified mitigation exists. Describe limitations plainly; distinguish planned, experimental, supported, and deprecated states.

## 2. Visual identity
Black/yellow is the foundation; red and cyan/blue are semantic accents.

| Token | HEX | RGB | HSL | Use |
|---|---|---|---|---|
| Yellow 500 | #FFCC00 | 255,204,0 | 48°,100%,50% | Primary accent |
| Yellow 400 | #FFE600 | 255,230,0 | 55°,100%,50% | Highlight/active |
| Black 950 | #000000 | 0,0,0 | 0°,0%,0% | Main dark surface |
| Black 900 | #1A1A1A | 26,26,26 | 0°,0%,10% | Elevated dark surface |
| Red 600 | #E53935 | 229,57,53 | 1°,77%,55% | Errors/destructive |
| Cyan 400 | #29B6F6 | 41,182,246 | 199°,92%,56% | Active process/data flow |
| Gray 100 | #F5F5F5 | 245,245,245 | 0°,0%,96% | Light surface |
| Gray 300 | #D6D6D6 | 214,214,214 | 0°,0%,84% | Borders |
| Gray 500 | #808080 | 128,128,128 | 0°,0%,50% | Secondary text |
| Gray 700 | #4A4A4A | 74,74,74 | 0°,0%,29% | Muted dark surface |
| White 000 | #FFFFFF | 255,255,255 | 0°,0%,100% | Text on dark |
| Yellow 100 | #FFF7CC | 255,247,204 | 52°,100%,90% | Soft highlight |
| Red 100 | #FFEBEE | 255,235,238 | 351°,100%,96% | Error background |
| Cyan 100 | #E1F5FE | 225,245,254 | 198°,100%,94% | Info background |
| Gray 900 | #262626 | 38,38,38 | 0°,0%,15% | Strong dark surface |
| Gray 050 | #FAFAFA | 250,250,250 | 0°,0%,98% | Page background |
| Yellow 700 | #B38F00 | 179,143,0 | 48°,100%,35% | Dark-mode accent |
| Red 800 | #B71C1C | 183,28,28 | 0°,74%,41% | Dark error |
| Cyan 700 | #0277BD | 2,119,189 | 202°,98%,37% | Dark info |
| Gray 200 | #EEEEEE | 238,238,238 | 0°,0%,93% | Dividers |
| Gray 600 | #666666 | 102,102,102 | 0°,0%,40% | Secondary dark text |

Target WCAG 2.2 AA contrast for text/controls; show keyboard focus; never rely on color alone. Verify each foreground/background pairing.

## 3. Motion system

Use motion to explain state changes, prefer transform/opacity, and respect `prefers-reduced-motion`.

### Ten element animations
| Pattern | Keyframe intent | Duration | Easing | Purpose |
|---|---|---:|---|---|
| Fade in | opacity 0→1 | 160ms | ease-out | Arrival |
| Rise in | opacity + translateY(8px→0) | 220ms | cubic-bezier(.2,.8,.2,1) | Cards/panels |
| Press | scale(1→.98→1) | 120ms | ease-out | Button feedback |
| Pulse | subtle opacity/scale cycle | 1.4s | ease-in-out | Live status |
| Shimmer | background-position sweep | 1.2s | linear | Loading skeleton |
| Spin | rotate(0→360deg) | 900ms | linear | Indeterminate progress |
| Draw | stroke-dashoffset full→0 | 600ms | ease-in-out | Vector path |
| Shake | small horizontal offsets | 260ms | ease-in-out | Invalid input |
| Expand | opacity + scale(.98→1) | 180ms | ease-out | Disclosure |
| Progress | scaleX(0→measured value) | Data-driven | linear | Measured progress |

### Ten transitions
| Transition | Duration | Easing | Purpose |
|---|---:|---|---|
| Color/background | 140ms | ease-out | Hover/active |
| Border/focus | 120ms | ease-out | Focus |
| Button press | 100ms | ease-in | Press |
| Tooltip | 120ms | ease-out | Tooltip |
| Popover | 160ms | ease-out | Menus |
| Modal | 200ms | cubic-bezier(.2,.8,.2,1) | Dialog |
| Drawer | 240ms | cubic-bezier(.2,.8,.2,1) | Side panel |
| Tab indicator | 180ms | ease-in-out | Navigation |
| Page content | 220ms | ease-out | Route swap |
| Alert | 180ms | ease-out | Notice |

Example:
```css
@keyframes duck-fade-in { from { opacity: 0 } to { opacity: 1 } }
@keyframes duck-rise-in { from { opacity: 0; transform: translateY(8px) } to { opacity: 1; transform: translateY(0) } }
@media (prefers-reduced-motion: reduce) {
  *, *::before, *::after { animation-duration: .01ms !important; transition-duration: .01ms !important; }
}
```

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
