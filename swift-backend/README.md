# NUN control-plane — Server-Side Swift

Swift 6 / Vapor management service kept separate from the existing system implementation. Run: `cd swift-backend && swift test && swift run nuncontrol-server`.

Configuration: `HOST`, `PORT`, `SERVICE_NAME`, `SERVICE_VERSION`. Credentials and signing keys must come from an external secret provider, never source code.

Endpoints: `GET /health`, `GET /ready`, `GET /v1/health`. Readiness checks process state only; dependency probes remain a production gate. Responses propagate `X-Trace-ID`.

**Critical boundary:** this is a management/control-plane process only. It must never be linked into the freestanding kernel, bootloader, interrupt handlers, HAL, or bare-metal targets. Kernel/boot communication must use explicit versioned IPC contracts and a separate host-side adapter.

Core contracts include immutable Sendable request context, validated Codable configuration, API error DTO, async repository boundary and trace/security-header middleware. Never block SwiftNIO event loops; use parameterized queries and bounded pools.

PostgreSQL/Redis, JWT/RBAC, TLS, OpenTelemetry, Prometheus, migrations, rate limiting, OpenAPI, load testing and Kubernetes are not yet claimed as production-ready.
