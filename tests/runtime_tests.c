#include "nun/runtime.h"

#include <assert.h>
#include <stdint.h>

typedef struct test_context {
    uint64_t ticks;
    unsigned initialized;
    unsigned polls;
    unsigned shutdowns;
} test_context_t;

static pbsc_status_t read_ticks(void *context, uint64_t *ticks_out)
{
    test_context_t *state = (test_context_t *)context;
    if (state == NULL || ticks_out == NULL) return PBSC_EINVAL;
    state->ticks += 1u;
    *ticks_out = state->ticks;
    return PBSC_OK;
}

static dlo_result_t app_init(void *context)
{
    test_context_t *state = (test_context_t *)context;
    if (state == NULL) return DLO_EINVAL;
    state->initialized += 1u;
    return DLO_OK;
}

static dlo_result_t app_step(void *context)
{
    test_context_t *state = (test_context_t *)context;
    if (state == NULL) return DLO_EINVAL;
    state->polls += 1u;
    return DLO_OK;
}

static void app_shutdown(void *context)
{
    test_context_t *state = (test_context_t *)context;
    if (state != NULL) state->shutdowns += 1u;
}

int main(void)
{
    test_context_t context = {0};
    unsigned char arena_storage[512];
    nun_runtime_t runtime;
    dlo_hooks_t hooks = { &context, app_init, app_step, app_shutdown };
    pbsc_platform_ops_t platform = {
        PBSC_ABI_VERSION,
        (uint32_t)sizeof(pbsc_platform_ops_t),
        PBSC_PLATFORM_CAP_MONOTONIC_CLOCK,
        &context,
        read_ticks,
        NULL,
        NULL,
        NULL
    };
    nun_runtime_config_t config = {
        &platform,
        &hooks,
        arena_storage,
        sizeof(arena_storage),
        256u,
        8u,
        100u,
        20u
    };
    void *memory;
    riae_health_state_t health = RIAE_DEGRADED;
    uint32_t stale = UINT32_MAX;

    assert(nun_runtime_init(&runtime, &config) == NUN_OK);
    assert(nun_runtime_state(&runtime) == NUN_STATE_READY);
    assert(nun_runtime_start(&runtime) == NUN_OK);
    assert(context.initialized == 1u);
    assert(nun_runtime_poll(&runtime) == NUN_OK);
    assert(context.polls == 1u);

    memory = nun_runtime_allocate(&runtime, 32u, 8u);
    assert(memory != NULL);
    assert(nun_runtime_reserve_queue_slots(&runtime, 2u) == NUN_OK);
    assert(nun_runtime_release_queue_slots(&runtime, 2u) == NUN_OK);
    assert(nun_runtime_reset_arena(&runtime) == NUN_OK);
    assert(nun_runtime_health(&runtime, &health, &stale) == NUN_OK);
    assert(health == RIAE_HEALTHY);
    assert(stale == 0u);

    assert(nun_runtime_authorize(NUN_ACCESS_READ_LOCAL, NUN_ORIGIN_USER) == 1);
    assert(nun_runtime_authorize(NUN_ACCESS_MUTATE_DEVICE, NUN_ORIGIN_USER) == 0);
    assert(nun_runtime_authorize(NUN_ACCESS_UPLOAD_DATA, NUN_ORIGIN_USER) == 0);

    assert(nun_runtime_stop(&runtime) == NUN_OK);
    assert(context.shutdowns == 1u);
    assert(nun_runtime_state(&runtime) == NUN_STATE_STOPPED);
    return 0;
}
