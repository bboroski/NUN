# NUN bounded event history

The fixed-capacity ledger preserves event sequence, monotonic observation time, event/source codes, severity, evidence flags, a bounded confidence annotation and an optional earlier-event reference. It uses static storage only, evicts oldest records on overflow, counts drops and produces read-only chronological snapshots.

This supports forensic boot and preflight review without capturing private payloads or changing the boot image/device. A corroborated flag must include an observed signal. Confidence is not a probability of truth; the ledger records evidence metadata, not causation.

Strict C contract tests run in GitHub Actions.
