# NUN Declarative Service Graph v1

**Status:** implemented as a host-testable C11 dependency planner. It is not yet a kernel service manager.

## Purpose

The service graph is the first executable slice of the declarative daemon/bootstrap idea. It validates a bounded set of service descriptors and produces a deterministic startup order without executing callbacks or touching hardware.

## Contract

Header: `include/nun/service_graph.h`  
Implementation: `src/services/graph.c`  
ABI version: `NUN_SERVICE_GRAPH_ABI_VERSION = 0x00010000`

Each node has a stable non-zero 32-bit ID, up to 16 dependency IDs, and zero reserved flags. A graph contains 1–64 nodes. Dependencies must exist in the same graph, be unique within a node, and may not refer to the node itself.

The planner uses Kahn's topological-sort algorithm. Whenever several nodes are ready, the node with the lowest service ID is selected. Therefore identical inputs yield identical output independent of the input array's ordering. The returned values are indices into the caller's descriptor array.

The output is staged in bounded stack storage and copied only after the entire graph validates. On failure, `order_count` is zero and the output buffer is left untouched. No heap allocation, dynamic loading, callback invocation, synchronization, process creation, or device access occurs.

## Errors

| Result | Meaning |
|---|---|
| `NUN_SERVICE_GRAPH_E_ARGUMENT` | Invalid pointer, zero service ID, non-zero reserved flags, or empty graph |
| `NUN_SERVICE_GRAPH_E_LIMIT` | More than 64 nodes, more than 16 dependencies, or missing dependency storage |
| `NUN_SERVICE_GRAPH_E_DUPLICATE_ID` | Two descriptors use the same ID |
| `NUN_SERVICE_GRAPH_E_DUPLICATE_DEPENDENCY` | One node repeats a dependency |
| `NUN_SERVICE_GRAPH_E_MISSING_DEPENDENCY` | A dependency ID is not present in the graph |
| `NUN_SERVICE_GRAPH_E_CYCLE` | A self-edge or dependency cycle prevents a topological order |
| `NUN_SERVICE_GRAPH_E_BUFFER` | Caller output buffer is smaller than the graph |

## Security and ownership

Descriptors, dependency arrays, and output storage remain caller-owned. This planner does not grant permissions, establish trust, start services, or turn health observations into recovery authority. A future supervisor must separately validate manifests, permissions, isolation, resource budgets, staged health checks, and atomic activation/rollback.

## Verification

`nun_service_graph_tests` covers deterministic ordering, stable tie-breaking, missing dependencies, duplicate IDs/edges, cycles, undersized output buffers, and the no-partial-output guarantee. Host tests do not establish kernel integration or target-hardware support.
