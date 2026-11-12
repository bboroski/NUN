#include "nun/service_graph.h"

#include <string.h>

static size_t nun_find_service(const nun_service_descriptor_t *services,
                               size_t service_count,
                               uint32_t service_id)
{
    size_t i;
    for (i = 0U; i < service_count; ++i) {
        if (services[i].service_id == service_id) {
            return i;
        }
    }
    return service_count;
}

static int nun_dependencies_emitted(const nun_service_descriptor_t *service,
                                    const nun_service_descriptor_t *services,
                                    size_t service_count,
                                    const uint8_t *emitted)
{
    size_t i;
    for (i = 0U; i < (size_t)service->dependency_count; ++i) {
        const size_t dependency_index = nun_find_service(
            services, service_count, service->dependency_ids[i]);
        if (dependency_index >= service_count || emitted[dependency_index] == 0U) {
            return 0;
        }
    }
    return 1;
}

nun_service_graph_result_t nun_service_graph_plan(
    const nun_service_descriptor_t *services,
    size_t service_count,
    size_t *order,
    size_t order_capacity,
    size_t *order_count)
{
    uint8_t emitted[NUN_SERVICE_GRAPH_MAX_SERVICES];
    size_t planned[NUN_SERVICE_GRAPH_MAX_SERVICES];
    size_t i;
    size_t j;
    size_t emitted_count = 0U;

    if (order_count == NULL) {
        return NUN_SERVICE_GRAPH_E_ARGUMENT;
    }
    *order_count = 0U;

    if (services == NULL || order == NULL || service_count == 0U) {
        return NUN_SERVICE_GRAPH_E_ARGUMENT;
    }
    if (service_count > NUN_SERVICE_GRAPH_MAX_SERVICES) {
        return NUN_SERVICE_GRAPH_E_LIMIT;
    }
    if (order_capacity < service_count) {
        return NUN_SERVICE_GRAPH_E_BUFFER;
    }

    memset(emitted, 0, sizeof(emitted));

    /* Validate node shape and IDs before resolving any edges. */
    for (i = 0U; i < service_count; ++i) {
        const nun_service_descriptor_t *service = &services[i];
        if (service->service_id == 0U || service->flags != 0U) {
            return NUN_SERVICE_GRAPH_E_ARGUMENT;
        }
        if (service->dependency_count > NUN_SERVICE_GRAPH_MAX_DEPENDENCIES ||
            (service->dependency_count > 0U && service->dependency_ids == NULL)) {
            return NUN_SERVICE_GRAPH_E_LIMIT;
        }
        for (j = 0U; j < i; ++j) {
            if (services[j].service_id == service->service_id) {
                return NUN_SERVICE_GRAPH_E_DUPLICATE_ID;
            }
        }
    }

    /* Validate edges, including duplicate and unresolved dependencies. */
    for (i = 0U; i < service_count; ++i) {
        const nun_service_descriptor_t *service = &services[i];
        for (j = 0U; j < (size_t)service->dependency_count; ++j) {
            size_t k;
            const uint32_t dependency_id = service->dependency_ids[j];
            if (dependency_id == 0U ||
                nun_find_service(services, service_count, dependency_id) >= service_count) {
                return NUN_SERVICE_GRAPH_E_MISSING_DEPENDENCY;
            }
            if (dependency_id == service->service_id) {
                return NUN_SERVICE_GRAPH_E_CYCLE;
            }
            for (k = 0U; k < j; ++k) {
                if (service->dependency_ids[k] == dependency_id) {
                    return NUN_SERVICE_GRAPH_E_DUPLICATE_DEPENDENCY;
                }
            }
        }
    }

    /* Kahn's algorithm with stable ID tie-breaking; stage output atomically. */
    while (emitted_count < service_count) {
        size_t selected = service_count;
        uint32_t selected_id = UINT32_MAX;

        for (i = 0U; i < service_count; ++i) {
            if (emitted[i] == 0U &&
                nun_dependencies_emitted(&services[i], services, service_count, emitted) != 0 &&
                (selected >= service_count || services[i].service_id < selected_id)) {
                selected = i;
                selected_id = services[i].service_id;
            }
        }

        if (selected >= service_count) {
            return NUN_SERVICE_GRAPH_E_CYCLE;
        }
        emitted[selected] = 1U;
        planned[emitted_count] = selected;
        ++emitted_count;
    }

    for (i = 0U; i < service_count; ++i) {
        order[i] = planned[i];
    }
    *order_count = service_count;
    return NUN_SERVICE_GRAPH_OK;
}
