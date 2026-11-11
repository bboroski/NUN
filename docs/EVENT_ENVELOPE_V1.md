# NUN Event Envelope v1

## Scope

NUN owns the host-side C11 ABI representation and bounded validation for the
federation event envelope. This is a data contract only: it does not implement
an event bus, queue, scheduler, transport, persistence layer or authorization
mechanism. The kernel remains independent of host-side services.

The shape is aligned with Klipot's `nun.event.v1` JSON Schema. The C ABI uses
fixed-capacity strings and a caller-owned payload to avoid allocation and
ownership ambiguity at the boundary.

## Contract

- ABI version: `NUN_EVENT_ABI_VERSION` (`0x00010000`).
- Required text: `event_id`, `producer`, and `kind`; each is a
  NUL-terminated UTF-8 string of 1–63 bytes.
- Optional text: `correlation_id`; an empty string means no correlation ID.
- Ordering: `sequence` is monotonic per producer. It is not a global order.
- Clock: `monotonic_ticks` is meaningful only in the producer's declared
  monotonic clock domain; consumers must not compare unrelated clock domains.
- Severity: trace, info, warning, error, critical.
- Classification: public, internal, sensitive, restricted.
- Payload: caller-owned serialized JSON object, 2–4096 bytes. The validator
  checks presence and bounds; the producer remains responsible for UTF-8 and
  JSON-object parsing before publication.

Example logical envelope:

```json
{
  "schema_version": "nun.event.v1",
  "event_id": "evt-0001",
  "producer": "nun.runtime",
  "sequence": 1,
  "clock": {"kind": "monotonic", "value": 42, "unit": "tick"},
  "severity": "info",
  "kind": "runtime.ready",
  "data_classification": "internal",
  "payload": {}
}
```

## Safety and compatibility

- `nun_event_validate_v1` is pure and bounded: it allocates no memory, performs
  no I/O, mutates no queue, and grants no permissions.
- Payload memory is not copied or freed. The caller must keep it alive while
  consumers use the event.
- Unknown ABI versions, incorrect structure sizes, unterminated required
  strings, invalid enum values, null payloads and out-of-range payload sizes
  are rejected with distinct result codes.
- A future incompatible layout requires a new ABI version and validation
  entry point; v1 callers must not reinterpret a newer structure.
- Classification is metadata, not an access-control decision. A transport or
  consumer must enforce its own policy before exposing restricted data.

## Build and acceptance

Build the host contract suite with:

```sh
cmake -S . -B build -DNUN_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Acceptance criteria:

1. The event contract compiles as C11 with the repository warning flags.
2. Valid v1 envelopes pass; invalid version, size, text, severity,
   classification and payload bounds fail deterministically.
3. Existing boot-info and runtime tests remain registered and unchanged.
4. No event transport, device operation, privileged action or kernel linkage is
   introduced by this increment.
