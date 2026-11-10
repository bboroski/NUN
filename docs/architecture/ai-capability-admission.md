# AI-Generated Capability Admission

**Status:** proposed architecture policy  
**Implementation status:** not implemented  
**Owner:** NUN capability-composition subsystem

## 1. Trust boundary

Gemini or another model may draft source code, schemas, manifests, tests and adapter proposals. These outputs are untrusted artifacts. A fluent explanation, model citation or generated test suite does not establish correctness, publisher identity, ABI compatibility or permission to execute.

The model is never an authority in the boot path, kernel privilege model, scheduler, memory map or capability grant process.

## 2. Admission pipeline

A model-originated capability follows the same controlled lifecycle as any other external capability:

discovered -> quarantined -> validated -> resolved -> staged -> active

The pipeline must:
- record immutable artifact digest, origin, version, license and signer information;
- validate the manifest schema and reject unknown or ambiguous fields;
- verify signatures under an explicit trust policy where signed artifacts are required;
- confirm target architecture, ABI, dependency constraints and resource profile;
- request only named, scoped permissions and reject privilege escalation;
- resolve dependency graphs deterministically and reject cycles/conflicts;
- stage the candidate in isolation with memory, CPU, I/O and time limits;
- activate by atomic registry snapshot replacement only after policy approval;
- retain rollback and revocation records.

Failed admission leaves the active registry unchanged.

## 3. No generated glue execution

The resolver must not invent adapters, evaluate source strings, run generated shell scripts or compile and load arbitrary code during admission. An adapter must be a separately versioned capability with a reviewed implementation and explicit interfaces on both sides.

## 4. Reproducibility

Every accepted capability must build and pass validation without a Gemini connection. Record the source revision, toolchain, build inputs, test results and artifact digest. A generated test is supplemental; independent boundary and negative tests are mandatory.

## 5. Required rejection cases

Reject missing provenance, altered digest, invalid signature where required, unsupported ABI, unbounded resource requests, undeclared side effects, unknown permissions, cyclic dependencies, duplicate identity/version with different bytes and artifacts that cannot be rolled back safely.

This document extends the concepts in [Capability Composition & Assimilation](capability-composition.md). It does not claim that the NUN kernel or admission runtime is implemented.
