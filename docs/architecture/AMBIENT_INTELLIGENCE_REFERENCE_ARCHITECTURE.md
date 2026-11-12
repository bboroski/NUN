# NUN Ambient Intelligence OS — Reference Architecture

**Status:** architecture proposal and repository implementation baseline.  
**Scope:** merge the two visual-reference workstreams: contextual operating surface and spatial/environmental intelligence.  
**Maturity:** documentation committed on a feature branch; runtime implementation remains gated by subsystem evidence.

## 1. Product objective

Build a modular operating environment that presents trusted, time-relevant information and optional spatial capabilities through one coherent system. The product is not an imitation of iOS. It combines:

- a contextual surface (time/date, notifications, media, weather/context, quick actions, system status);
- an emergency information path with source verification, expiry, history and explicit unknown/offline states;
- an optional visual/spatial stack (image buffers, camera sessions, scene metadata, geometry and environmental analysis);
- a replaceable UI layer and SDK for third-party applications;
- a federated capability catalog across independently maintained repositories.

## 2. Repository ownership

| Repository | Owns | Must not own |
|---|---|---|
| NUN | Kernel primitives, memory, scheduling, IPC, device abstraction | AI models, UI policy, network providers |
| NUNLauncher | Boot admission/supervisor; future user-session shell is a separate target | Kernel internals, unverified firmware handoff |
| Kloz | Canonical schemas, ABI and compatibility rules | Runtime service execution |
| CO | Resource quotas, service lifecycle and composition | Unbounded or privileged services |
| SaintSI | Runtime adapters and provider bridges | Assumed hardware/provider availability |
| SAMBU | SDKs, language tooling, samples and generators | Privileged kernel access |
| Luci | Accessible UI components and design tokens | Data provenance and security policy |
| Klipot | Capability registry, dependency/evidence validation | Trusting unverified manifests |
| ToolNG | Inspectors, validators, fixtures and simulators | Real-world emergency dispatch |
| Hellduck | Federation, version pins and service discovery | Automatic code execution or trust inheritance |
| ReaperCripple | Device provisioning/security boundaries | Covert capture or unsupported attestation claims |
| Fragile-Baby | Recovery, bounded persistence and fault isolation | Inventing a verified emergency clear state |
| STaas (proposed) | Optional hosted integration/management product | Mandatory cloud dependency for local OS functions |

STaas is not currently present in the accessible repository inventory. Treat it as a proposed product until a repository is created and ownership/license are established.

## 3. Layered architecture

```text
Applications / Desktop / Widgets
       |            |
   SAMBU SDK     Luci UI
       |            |
  Versioned Kloz contracts
       |
 User-space services and adapters
  SaintSI | CO | Vision providers
       |
 Capability registry / federation
  Klipot | Hellduck | optional STaas
       |
 Diagnostics / recovery / security
 ToolNG | Fragile-Baby | ReaperCripple
       |
 NUN kernel primitives and device boundaries
       |
 Hardware (optional camera/GPU/NPU/sensors)
```

The dependency direction is one-way. The kernel must boot and operate without the vision stack, cloud services, weather, calendar, media providers or graphical shell.

## 4. Contextual system surface

The shell may compose the following independent cards:

- Clock and date with locale/time-zone correctness.
- Notification groups with priority, deduplication, expiry and per-source controls.
- Media session controls through standard session contracts.
- Weather and calendar cards from explicitly configured providers.
- Network, battery, storage and system-health indicators.
- Quick actions whose availability is discovered from declared capabilities.
- Emergency banner and event timeline driven only by authenticated/approved providers.

Every card must support loading, empty, stale, unavailable, denied and error states. Cards may not infer a healthy state from missing data.

## 5. Emergency event safety

Emergency information is a safety-sensitive feed, not a generic notification category.

Required fields: event ID, issuer/source ID, source authentication result, geographic scope if supplied by the issuer, event type, issued-at, received-at, expiry/valid-until, sequence number, signature/provenance reference and correction/cancellation relation.

Rules:
1. Only configured trusted issuers can produce a verified state.
2. A timeout or offline condition becomes **unknown/stale**, never **clear**.
3. Expired events remain in history but are not shown as current.
4. Duplicate and out-of-order events are idempotently handled.
5. UI must distinguish verified, unverified, stale, unavailable and corrected events.
6. No location is transmitted unless explicitly required, disclosed and authorized.
7. The system never substitutes weather inference, crowdsourced rumor or model output for an official alert.

## 6. Spatial and visual data path

```text
Device capability discovery
       |
Camera / imported image / video / sensor source
       |
Owned frame buffer + explicit pixel format/stride
       |
Optional image pipeline (CPU/GPU/NPU provider)
       |
Typed observations with confidence + provenance
       |
Scene graph / geometry / coordinate transforms
       |
Consumer API -> UI overlay / diagnostics / application
```

Spatial envelope must include coordinate frame, units, handedness, origin, calibration revision, timestamp, source identity, confidence and validity interval. Frame descriptors must include dimensions, format, plane layout, stride, ownership, lifetime and synchronization state.

The first implementation target should be deterministic imported-image processing and synthetic fixtures. Live camera and hardware acceleration are later optional providers.

## 7. Privacy and threat boundaries

- Camera and sensor access is capability-based, visible and revocable.
- No face identification, covert capture or persistent person tracking is in scope.
- Raw frames are transient by default; persistence requires an explicit user action/policy.
- External providers are opt-in and isolated behind adapters.
- Plugins are signed/validated and sandboxed; repository membership is not trust.
- Telemetry is minimized, locally inspectable and disabled unless policy explicitly enables it.
- Model output is advisory and must not create or clear emergency states.

## 8. Performance and reliability constraints

- All queues, payloads, caches and retained histories have explicit limits.
- Back-pressure, cancellation and expiry are first-class.
- Slow or failed vision providers cannot block input, boot, notifications or emergency rendering.
- Services can degrade independently; local clock and core controls remain available offline.
- GPU/NPU use is opportunistic and never assumed.
- Every async event has correlation and idempotency identifiers.

## 9. Implementation milestones

**M0 — Contract baseline:** schemas, ABI, fixtures, ownership, version policy.  
**M1 — Host reference implementation:** bounded event envelope, event validator, synthetic provider and deterministic tests.  
**M2 — User-space service layer:** notification/event broker, media/context adapters, service lifecycle.  
**M3 — UI integration:** accessible dashboard, notification center, media card, emergency timeline and system status.  
**M4 — Spatial reference pipeline:** imported-image frames, metadata, geometry transforms, fixture-based scene graph.  
**M5 — Optional providers:** camera, GPU/NPU, weather/calendar, remote services, only after explicit qualification.  
**M6 — Federation and packaging:** compatibility manifests, release pins, provenance and integration evidence.

## 10. Required evidence for each capability

Track each capability as one of: `proposed`, `specified`, `implemented`, `tested`, `integrated`, `hardware-qualified`. Status may advance only with an artifact:

- proposed: accepted scope and owner;
- specified: versioned contract and failure semantics;
- implemented: code in a reviewed branch;
- tested: reproducible tests and output;
- integrated: consumer/provider compatibility test;
- hardware-qualified: target-specific hardware evidence.

A mockup, README, open PR or host test is not hardware qualification.

## 11. Workstream catalogue model

The original target is 666 repository-owned work items per participating repository, arranged as 18 workstreams × 37 items. This is a backlog sizing convention, not a claim of completed functionality. Each item must have a unique repository-scoped ID, concise capability statement, owner, dependencies, contract version, risk, test plan, acceptance criteria and evidence link. Duplicate shells or generated filler do not count as valid innovations.

The 18 workstreams are:
1. Context/event model
2. Notification lifecycle
3. Emergency provenance and expiry
4. Media/session integration
5. Clock/calendar/context providers
6. Widget/component contracts
7. Spatial coordinate model
8. Image/frame-buffer pipeline
9. Camera/sensor capability model
10. Lighting and photometric analysis
11. Reflection and surface cues
12. Material and object metadata
13. Scene graph and geometry
14. Optional 3D reconstruction
15. Privacy and permission boundaries
16. Resource limits and degraded mode
17. Diagnostics, simulation and test evidence
18. Federation, packaging and release compatibility

## 12. Definition of done

- Existing project tests/builds remain green.
- Contracts have versioned schemas and negative fixtures.
- No repository takes ownership outside its defined boundary.
- Offline, stale, permission-denied and provider-failure paths are tested.
- No unsupported hardware or AI claims appear in documentation.
- Changes are delivered through focused PRs and are not merged over failing checks or unresolved review threads.
