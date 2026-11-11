#include "nun/runtime.h"

#include <assert.h>
#include <stdint.h>

typedef struct test_context {
    uint64_t ticks;
    unsigned initialized;
    unsigned polls;
    unsigned shutdowns;
} test_context_t;

static kloz_status_t read_ticks(void *context, uint64_t *ticks_out)
{
    test_context_t *state = (test_context_t *)context;
    if (state == NULL || ticks_out == NULL) return KLOZ_EINVAL;
    state->ticks += 1u;
    *ticks_out = state->ticks;
    return KLOZ_OK;
}

static co_result_t app_init(void *context)
{
    test_context_t *state = (test_context_t *)context;
    if (state == NULL) return CO_EINVAL;
    state->initialized += 1u;
    return CO_OK;
}

static co_result_t app_step(void *context)
{
    test_context_t *state = (test_context_t *)context;
    if (state == NULL) return CO_EINVAL;
    state->polls += 1u;
    return CO_OK;
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
    co_hooks_t hooks = { &context, app_init, app_step, app_shutdown };
    kloz_platform_ops_t platform = {
        KLOZ_ABI_VERSION,
        (uint32_t)sizeof(kloz_platform_ops_t),
        KLOZ_PLATFORM_CAP_MONOTONIC_CLOCK,
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
    saintsi_health_state_t health = SAINTSI_DEGRADED;
    uint32_t stale = UINT32_MAX;

    assert(nun_runtime_init(&runtime, &config) == NUN_OK);
    assert(nun_runtime_state(&runtime) == NUN_STATE_READY);
    assert(nun_runtime_start(&runtime) == NUN_OK);
    assert(context.initialized == 1u);
    assert(nun_runtime_poll(&runtime) == NUN_OK);
    assert(context.polls == 1u);

    memory = nun_runtime_allocate(&runtime, 32u, 8u);
    assert(memory != NULL);
    assert(runtime.arena_reserved_bytes == 32u);
    /* The configured 256-byte budget has only 224 bytes left. */
    assert(nun_runtime_allocate(&runtime, 225u, 8u) == NULL);
    assert(runtime.arena_reserved_bytes == 32u);
    assert(nun_runtime_reserve_queue_slots(&runtime, 2u) == NUN_OK);
    assert(nun_runtime_release_queue_slots(&runtime, 2u) == NUN_OK);
    assert(nun_runtime_reset_arena(&runtime) == NUN_OK);
    assert(runtime.arena_reserved_bytes == 0u);
    assert(nun_runtime_health(&runtime, &health, &stale) == NUN_OK);
    assert(health == SAINTSI_HEALTHY);
    assert(stale == 0u);

    assert(nun_runtime_authorize(NUN_ACCESS_READ_LOCAL, NUN_ORIGIN_USER) == 1);
    assert(nun_runtime_authorize(NUN_ACCESS_MUTATE_DEVICE, NUN_ORIGIN_USER) == 0);
    assert(nun_runtime_authorize(NUN_ACCESS_UPLOAD_DATA, NUN_ORIGIN_USER) == 0);

    assert(nun_runtime_stop(&runtime) == NUN_OK);
    assert(context.shutdowns == 1u);
    assert(nun_runtime_state(&runtime) == NUN_STATE_STOPPED);
    return 0;
}
