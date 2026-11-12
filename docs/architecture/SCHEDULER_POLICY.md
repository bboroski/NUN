# NUN scheduler policy core

**Status:** host-tested policy implementation; not yet connected to kernel context switching.

## Scope
A C11 module with fixed storage and no dynamic allocator or hardware access. It selects the task ID that should receive the next CPU quantum.

- Maximum 64 task records; a full table returns EFULL.
- Eight priority levels (0 lowest, 7 highest).
- Caller-supplied monotonic ticks and a fixed quantum.
- Round-robin tie-breaking through a rotating slot cursor.
- Optional priority aging, capped at level 7.
- Explicit READY, RUNNING and BLOCKED transitions; yield, wake and terminate operations.
- Invalid transitions, duplicate IDs, invalid priorities and clock regressions are rejected.
- Dispatch and per-task run counters saturate instead of wrapping.

## Semantics
The current task retains the CPU until its quantum expires. At expiry it returns to READY and the highest effective-priority READY task is selected. Equal-priority tasks are selected in round-robin slot order. Aging increments effective priority for each full waiting interval. With no runnable task, pick-next returns ENO_RUNNABLE; the caller may idle or wait for an interrupt.

## Deliberate limits
This is a scheduling policy, not a complete kernel scheduler. It does not create address spaces, allocate or switch stacks, save registers/FPU/SIMD/TLS state, perform interrupt-return switching, preempt tasks, synchronize multiple CPUs, implement affinity/NUMA/deadlines, or use an AI model. The kernel must not claim preemptive scheduling until those paths have implementation and QEMU evidence.

## Build and test
The host CMake target is NUN.SchedulerPolicy. Tests are registered as nun_scheduler_policy_tests. The freestanding boot target is unchanged in this increment.
