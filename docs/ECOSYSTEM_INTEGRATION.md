# NUN ecosystem integration contract

**Principle:** preserve independent repositories, consume their capabilities through explicit APIs, and keep kernel code freestanding. Registry presence never grants runtime authority.

## 1. Repository ownership

| Exact repository name | Capability reused | Integration boundary |
|---|---|---|
| NUN | kernel, boot protocol, ABI and system-service host | canonical OS repository |
| NUNLauncher | Android HOME activity and app catalog | separate Android application; future authenticated bridge |
| Kloz | fixed-width status ABI, caller-owned arena, platform operations and resource budgets | pinned C submodule compiled into NUN kernel; bounded smoke checks |
| CO | lifecycle state machine, one-step polling, fault retention and cleanup | pinned C submodule compiled into NUN kernel; lifecycle smoke check |
| SaintSI | fixed-capacity heartbeat registry, timeout observation and CRC32 | pinned C submodule compiled into NUN kernel; health/integrity smoke check |
| ReaperCripple | BSP profiles, capability-gated boot stages, rollback and calibration | pinned submodule; only portable supervisor state machine is compiled/tested in generic kernel; hardware BSP remains opt-in |
| SAMBU | language definition, developer IDE, policy/workflow tooling and editor support | pinned submodule; user-space SDK and compiler tooling |
| ToolNG | bounded evidence ledger, local diagnostics, report model and allow-listed workflows | pinned submodule; portable schemas and diagnostic model only, Windows code remains separate |
| Klipot | manifest validation, dependency planning, capability catalog and operator UI | pinned submodule; user-space registry/control plane |

## 2. Source pinning and dependency policy

The seven companion repositories are declared in .gitmodules, pinned as gitlinks and recorded by commit in ecosystem/repos.lock.json. Run git submodule update --init --recursive or simply make kernel to initialize sources.

The NUN kernel currently compiles source from Kloz, CO, SaintSI and the portable ReaperCripple runtime supervisor. These calls are exercised in a bounded startup smoke check. This does not make the whole upstream repositories kernel code.

- Kloz does not assume an OS heap, scheduler or clock that NUN has not provided.
- CO does not create threads; the future NUN scheduler owns execution contexts and serializes runtime access.
- SaintSI reports health; application policy decides recovery. A heartbeat is not proof of correct operation.
- ReaperCripple board adapters are opt-in; pinout, voltage, timing, DMA and reset semantics require board evidence.
- SAMBU's Python-hosted interpreter and GUI run in user space, never in kernel privilege.
- ToolNG remains a native Windows product. Only portable schemas and reviewed policy/evidence concepts can be adapted.
- Klipot plans dependencies from declarative manifests; it does not execute plans or load arbitrary plugins.
- NUNLauncher stays an Android application and uses only documented, authenticated, versioned interfaces.

## 3. System event envelope

Cross-process events use a versioned envelope, not a raw C struct.

~~~json
{
  "schema": "nun.event.v1",
  "producer": "nun.memory",
  "sequence": 42,
  "monotonic_ns": 1200000000,
  "kind": "memory.page-allocation",
  "classification": "system-metadata",
  "payload": {"pages": 1, "result": "ok"}
}
~~~

The sample is illustrative. Sequence numbers are per producer. Timestamp source and unit must be declared. Event buffers are bounded and expose dropped-event counters. Default telemetry excludes credentials, personal paths, raw device identifiers and secret key material.

## 4. Lifecycle and resource rules

1. Validate a complete manifest and dependency graph before the first side effect.
2. Reject missing required capabilities; optional capabilities require an explicit degraded profile.
3. Reserve memory/queue/work budgets before admitting a task.
4. Start components in dependency order and unwind acquired resources in reverse order.
5. Publish health only after the required progress condition completes.
6. Treat missing telemetry as unknown, not healthy.
7. An in-process callback is not a sandbox; untrusted extensions require process isolation, syscall mediation and resource quotas.
8. Recovery actions are allow-listed, audited and owned by the relevant subsystem.

## 5. Integration maturity

M0 currently compiles Kloz, CO, SaintSI and the ReaperCripple supervisor source into the x86-64 kernel and exercises their platform-independent contracts during early initialization. SAMBU, ToolNG and Klipot remain pinned and described as user-space integrations; no Python, Windows GUI or HTML/JS code enters ring 0. NUNLauncher is a separately built Android HOME app.

Process isolation, IPC, actual driver qualification, compiler target ports and the graphical system shell remain future milestones.
