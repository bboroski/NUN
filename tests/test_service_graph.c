#include "nun/service_graph.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static void test_deterministic_topological_order(void)
{
    static const uint32_t app_deps[] = {20U, 10U};
    static const uint32_t storage_deps[] = {10U};
    const nun_service_descriptor_t services[] = {
        {30U, app_deps, 2U, 0U},
        {20U, storage_deps, 1U, 0U},
        {10U, NULL, 0U, 0U}
    };
    size_t order[3] = {99U, 99U, 99U};
    size_t count = 99U;

    assert(nun_service_graph_plan(services, 3U, order, 3U, &count) == NUN_SERVICE_GRAPH_OK);
    assert(count == 3U);
    assert(services[order[0]].service_id == 10U);
    assert(services[order[1]].service_id == 20U);
    assert(services[order[2]].service_id == 30U);
}

static void test_stable_tie_breaking_and_max_id(void)
{
    const nun_service_descriptor_t services[] = {
        {UINT32_MAX, NULL, 0U, 0U}, {20U, NULL, 0U, 0U}, {30U, NULL, 0U, 0U}
    };
    size_t order[3];
    size_t count = 0U;

    assert(nun_service_graph_plan(services, 3U, order, 3U, &count) == NUN_SERVICE_GRAPH_OK);
    assert(services[order[0]].service_id == 20U);
    assert(services[order[1]].service_id == 30U);
    assert(services[order[2]].service_id == UINT32_MAX);
}

static void test_missing_dependency_and_no_partial_output(void)
{
    static const uint32_t missing[] = {99U};
    const nun_service_descriptor_t services[] = {{1U, missing, 1U, 0U}};
    size_t order[1] = {77U};
    size_t count = 55U;

    assert(nun_service_graph_plan(services, 1U, order, 1U, &count) ==
           NUN_SERVICE_GRAPH_E_MISSING_DEPENDENCY);
    assert(count == 0U);
    assert(order[0] == 77U);
}

static void test_duplicate_ids_and_edges(void)
{
    static const uint32_t duplicate_edges[] = {1U, 1U};
    const nun_service_descriptor_t duplicate_ids[] = {
        {1U, NULL, 0U, 0U}, {1U, NULL, 0U, 0U}
    };
    const nun_service_descriptor_t duplicate_deps[] = {
        {1U, NULL, 0U, 0U}, {2U, duplicate_edges, 2U, 0U}
    };
    size_t order[2];
    size_t count = 0U;

    assert(nun_service_graph_plan(duplicate_ids, 2U, order, 2U, &count) ==
           NUN_SERVICE_GRAPH_E_DUPLICATE_ID);
    assert(nun_service_graph_plan(duplicate_deps, 2U, order, 2U, &count) ==
           NUN_SERVICE_GRAPH_E_DUPLICATE_DEPENDENCY);
}

static void test_cycle_and_buffer_limits(void)
{
    static const uint32_t first_dep[] = {2U};
    static const uint32_t second_dep[] = {1U};
    const nun_service_descriptor_t cycle[] = {
        {1U, first_dep, 1U, 0U}, {2U, second_dep, 1U, 0U}
    };
    const nun_service_descriptor_t valid[] = {{1U, NULL, 0U, 0U}};
    size_t order[2];
    size_t count = 0U;

    assert(nun_service_graph_plan(cycle, 2U, order, 2U, &count) == NUN_SERVICE_GRAPH_E_CYCLE);
    assert(count == 0U);
    assert(nun_service_graph_plan(valid, 1U, order, 0U, &count) == NUN_SERVICE_GRAPH_E_BUFFER);
    assert(count == 0U);
}

int main(void)
{
    test_deterministic_topological_order();
    test_stable_tie_breaking_and_max_id();
    test_missing_dependency_and_no_partial_output();
    test_duplicate_ids_and_edges();
    test_cycle_and_buffer_limits();
    (void)puts("NUN service graph tests passed");
    return 0;
}
