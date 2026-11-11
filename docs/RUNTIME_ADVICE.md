# NUN runtime advice

The host-side NUN runtime exposes `nun_runtime_get_advice()` for dashboards and operator tooling. The snapshot combines lifecycle state, available memory/queue/work budgets, heartbeat health and rollback-failure evidence into stable state/action keys and explicit start/poll/stop permissions.

The API is observational: it does not poll the runtime, run callbacks, reset budgets or recover resources. A non-zero rollback-failed mask takes precedence and remains a manual recovery blocker. This feature describes the existing host runtime; it does not claim a bootable kernel, firmware handoff or hardware qualification.
