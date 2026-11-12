# MoonTreach

This repository uses the canonical MoonTreach engine and reusable workflow hosted by `bboroski/Hellduck`, pinned to immutable core revision `edc139b8e08f098b708ce846ac0af60f206e7efb`.

- Computes changed paths from the exact base/head commit range.
- Selects checks mapped to changed files; unknown paths, missing diffs, oversized changes and merge-group candidates fail closed to the configured full suite.
- Executes repository-native commands as argument arrays without shell interpolation, with per-command timeouts.
- Runs the full configured suite for GitHub `merge_group` candidates.
- Uploads a machine-readable evidence report with SHAs, changed files, matched rules, selected checks, outcomes and elapsed time.

The authoritative contract is root-level `moontreach.json`. The workflow checks the exact PR merge ref or synthetic merge-group commit, not an unrelated branch head.

The caller has read-only `contents` permission. Checkouts do not persist credentials and no repository secrets are passed. MoonTreach does not merge PRs, rewrite branches, resolve semantic conflicts or bypass branch protection. Require this check in branch protection only after reviewing actual results; existing checks and human review remain authoritative.

Local engine commands are documented in the pinned Hellduck core at `tools/moontreach/engine.py`.
