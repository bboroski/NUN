# Octopus Ippon Release Protocol — NUN

This workflow is a release gate, not a claim that NUN is a production-ready OS.

## Entry points
- Pull requests and ordinary pushes continue to use the repository's existing CI workflows.
- `workflow_dispatch` runs validation and stores evidence without publishing a release.
- A pushed `v*` tag runs the same gates and publishes a GitHub Release only after all required jobs pass.

## Release gates
1. Checkout the exact tagged source, including submodules.
2. Run host contract tests (`make test`).
3. Build the Multiboot2 image and boot it in QEMU; require serial assertions for kernel entry, memory self-test, readiness and ping/pong.
4. Build the documented Limine ISO (`make iso`).
5. Stage both ISO targets, SHA-256 checksums and an SPDX JSON SBOM.
6. Generate GitHub build-provenance attestations for the release payload.
7. Upload evidence; publish only for a version tag.

## Safety boundaries
- No USB writing, firmware flashing, deployment to physical hardware or destructive operation is part of CI.
- QEMU success is not hardware qualification.
- A failed gate blocks publication; it is never converted to a warning.
- The workflow does not push commits, create tags or modify `main`.
- Version tags must be created deliberately after reviewing the candidate commit and CI evidence.

## 666-iteration rollout model
The 666 count is a controlled improvement backlog, not 666 fake test passes or 666 commits. Track each real change as an iteration with: an observable defect or risk, a minimal patch, a reproducible test, captured evidence, and a rollback path. The release workflow is the recurring acceptance gate for every iteration. Expand the rollout only after the current repository's workflow has passed on GitHub.
