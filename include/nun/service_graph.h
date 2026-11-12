#ifndef NUN_SERVICE_GRAPH_H
#define NUN_SERVICE_GRAPH_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUN_SERVICE_GRAPH_ABI_VERSION UINT32_C(0x00010000)
#define NUN_SERVICE_GRAPH_MAX_SERVICES 64U
#define NUN_SERVICE_GRAPH_MAX_DEPENDENCIES 16U

typedef enum nun_service_graph_result {
    NUN_SERVICE_GRAPH_OK = 0,
    NUN_SERVICE_GRAPH_E_ARGUMENT = -1,
    NUN_SERVICE_GRAPH_E_LIMIT = -2,
    NUN_SERVICE_GRAPH_E_DUPLICATE_ID = -3,
    NUN_SERVICE_GRAPH_E_DUPLICATE_DEPENDENCY = -4,
    NUN_SERVICE_GRAPH_E_MISSING_DEPENDENCY = -5,
    NUN_SERVICE_GRAPH_E_CYCLE = -6,
    NUN_SERVICE_GRAPH_E_BUFFER = -7
} nun_service_graph_result_t;

/*
 * A declarative service node. IDs are stable non-zero integers. Dependencies
 * refer to service IDs in the same graph; the caller owns all storage.
 */
typedef struct nun_service_descriptor {
    uint32_t service_id;
    const uint32_t *dependency_ids;
    uint16_t dependency_count;
    uint16_t flags; /* Reserved; must be zero in ABI v1. */
} nun_service_descriptor_t;

/*
 * Validate and produce a deterministic topological order.
 *
 * order receives indices into services, not service IDs. Among all nodes
 * whose dependencies are already emitted, the lowest service_id is selected.
 * No output is written unless the complete graph is valid. order_count is
 * set to zero on entry and to service_count on success.
 *
 * This function performs no allocation, callbacks, I/O, locking, or execution.
 * It is a planning primitive, not a service manager or kernel scheduler.
 */
nun_service_graph_result_t nun_service_graph_plan(
    const nun_service_descriptor_t *services,
    size_t service_count,
    size_t *order,
    size_t order_capacity,
    size_t *order_count);

#ifdef __cplusplus
}
#endif

#endif
