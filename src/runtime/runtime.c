#include "nun/runtime.h"

#include <limits.h>
#include <string.h>

#include "sambu_engine.h"

_Static_assert(NUN_ACCESS_READ_LOCAL == WDEP_OP_READ_LOCAL, "ToolNG read ABI drift");
_Static_assert(NUN_ACCESS_SCAN == WDEP_OP_SCAN, "ToolNG scan ABI drift");
_Static_assert(NUN_ACCESS_MUTATE_DEVICE == WDEP_OP_MUTATE_DEVICE, "ToolNG mutation ABI drift");
_Static_assert(NUN_ORIGIN_INTERNAL == WDEP_ORIGIN_INTERNAL, "ToolNG internal origin ABI drift");
_Static_assert(NUN_ORIGIN_USER == WDEP_ORIGIN_USER, "ToolNG user origin ABI drift");

static int32_t nun_read_tick(nun_runtime_t *runtime, uint32_t *tick_out)
{
    uint64_t ticks = 0u;

    if (runtime == NULL || tick_out == NULL ||
        runtime->platform.ticks_read == NULL) {
        return NUN_EINVAL;
    }
    if (runtime->platform.ticks_read(runtime->platform.context, &ticks) != KLOZ_OK) {
        return NUN_EPLATFORM;
    }

    *tick_out = (uint32_t)ticks;
    runtime->last_tick = *tick_out;
    return NUN_OK;
}

static int32_t nun_boot_platform(void *context)
{
    nun_runtime_t *runtime = (nun_runtime_t *)context;
    uint32_t now = 0u;

    if (runtime == NULL ||
        kloz_platform_validate(&runtime->platform) != KLOZ_OK ||
        (runtime->platform.capabilities & KLOZ_PLATFORM_CAP_MONOTONIC_CLOCK) == 0u) {
        return NUN_EPLATFORM;
    }
    return nun_read_tick(runtime, &now);
}

static int32_t nun_rollback_platform(void *context)
{
    (void)context;
    return 0;
}

static int32_t nun_boot_resources(void *context)
{
    nun_runtime_t *runtime = (nun_runtime_t *)context;

    if (runtime == NULL) return NUN_EINVAL;
    if (kloz_budget_init(&runtime->budget, runtime->memory_limit,
                         runtime->queue_limit, runtime->work_limit) != 0) {
        return NUN_EBUDGET;
    }
    kloz_arena_init(&runtime->arena, runtime->arena_storage,
                    runtime->memory_limit);
    return 0;
}

static int32_t nun_rollback_resources(void *context)
{
    nun_runtime_t *runtime = (nun_runtime_t *)context;

    if (runtime == NULL) return NUN_EINVAL;
    kloz_arena_reset(&runtime->arena);
    runtime->arena_reserved_bytes = 0u;
    memset(&runtime->budget, 0, sizeof(runtime->budget));
    return 0;
}

static int32_t nun_boot_lifecycle(void *context)
{
    nun_runtime_t *runtime = (nun_runtime_t *)context;

    if (runtime == NULL) return NUN_EINVAL;
    return co_runtime_init(&runtime->lifecycle, &runtime->lifecycle_hooks);
}

static int32_t nun_rollback_lifecycle(void *context)
{
    nun_runtime_t *runtime = (nun_runtime_t *)context;
    co_result_t result;

    if (runtime == NULL) return NUN_EINVAL;
    result = co_runtime_stop(&runtime->lifecycle);
    memset(&runtime->lifecycle, 0, sizeof(runtime->lifecycle));
    return result == CO_OK ? 0 : (int32_t)result;
}

static int32_t nun_boot_assurance(void *context)
{
    nun_runtime_t *runtime = (nun_runtime_t *)context;

    if (runtime == NULL) return NUN_EINVAL;
    saintsi_supervisor_init(&runtime->assurance);
    return saintsi_register_component(&runtime->assurance,
                                   NUN_HEALTH_COMPONENT_ID,
                                   runtime->heartbeat_timeout_ticks,
                                   runtime->last_tick);
}

static int32_t nun_rollback_assurance(void *context)
{
    nun_runtime_t *runtime = (nun_runtime_t *)context;

    if (runtime == NULL) return NUN_EINVAL;
    memset(&runtime->assurance, 0, sizeof(runtime->assurance));
    return 0;
}

nun_result_t nun_runtime_init(nun_runtime_t *runtime,
                              const nun_runtime_config_t *config)
{
    int32_t result;

    if (runtime == NULL || config == NULL ||
        config->platform == NULL || config->lifecycle_hooks == NULL ||
        config->lifecycle_hooks->init == NULL ||
        config->lifecycle_hooks->step == NULL ||
        config->arena_storage == NULL || config->arena_capacity == 0u ||
        config->memory_limit == 0u ||
        config->memory_limit > config->arena_capacity ||
        config->heartbeat_timeout_ticks == 0u ||
        config->heartbeat_timeout_ticks > (uint32_t)INT32_MAX) {
        return NUN_EINVAL;
    }

    memset(runtime, 0, sizeof(*runtime));
    runtime->platform = *config->platform;
    runtime->lifecycle_hooks = *config->lifecycle_hooks;
    runtime->arena_storage = config->arena_storage;
    runtime->arena_capacity = config->arena_capacity;
    runtime->arena_reserved_bytes = 0u;
    runtime->memory_limit = config->memory_limit;
    runtime->queue_limit = config->queue_limit;
    runtime->work_limit = config->work_limit;
    runtime->heartbeat_timeout_ticks = config->heartbeat_timeout_ticks;
    runtime->state = NUN_STATE_COLD;

    if (kloz_platform_validate(&runtime->platform) != KLOZ_OK ||
        (runtime->platform.capabilities & KLOZ_PLATFORM_CAP_MONOTONIC_CLOCK) == 0u ||
        runtime->platform.ticks_read == NULL) {
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EPLATFORM;
    }

    runtime->boot_steps[0] = (reaper_boot_step_t){
        "platform-contract", 0u, nun_boot_platform,
        nun_rollback_platform, runtime
    };
    runtime->boot_steps[1] = (reaper_boot_step_t){
        "resource-substrate", 0u, nun_boot_resources,
        nun_rollback_resources, runtime
    };
    runtime->boot_steps[2] = (reaper_boot_step_t){
        "component-lifecycle", 0u, nun_boot_lifecycle,
        nun_rollback_lifecycle, runtime
    };
    runtime->boot_steps[3] = (reaper_boot_step_t){
        "health-assurance", 0u, nun_boot_assurance,
        nun_rollback_assurance, runtime
    };

    if (reaper_runtime_init(&runtime->bootstrap) != REAPER_OK) {
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EBOOT;
    }

    result = reaper_runtime_bootstrap(&runtime->bootstrap,
                                     runtime->boot_steps, 4u, 0u);
    if (result != REAPER_OK) {
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EBOOT;
    }

    runtime->initialized = 1u;
    runtime->state = NUN_STATE_READY;
    return NUN_OK;
}

nun_result_t nun_runtime_start(nun_runtime_t *runtime)
{
    if (runtime == NULL || runtime->initialized == 0u ||
        runtime->state != NUN_STATE_READY) {
        return NUN_ESTATE;
    }

    if (co_runtime_start(&runtime->lifecycle) != CO_OK) {
        (void)reaper_runtime_transition(&runtime->bootstrap,
                                        REAPER_RUNTIME_EVENT_FAULT,
                                        REAPER_ERR_STATE);
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EIO;
    }
    if (reaper_runtime_transition(&runtime->bootstrap,
                                  REAPER_RUNTIME_EVENT_START,
                                  REAPER_OK) != REAPER_OK) {
        (void)co_runtime_stop(&runtime->lifecycle);
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EBOOT;
    }

    runtime->state = NUN_STATE_RUNNING;
    return NUN_OK;
}

nun_result_t nun_runtime_poll(nun_runtime_t *runtime)
{
    uint32_t now = 0u;
    saintsi_health_state_t health_state = SAINTSI_HEALTHY;
    uint32_t stale_count = 0u;
    int32_t result;

    if (runtime == NULL || runtime->initialized == 0u ||
        (runtime->state != NUN_STATE_RUNNING &&
         runtime->state != NUN_STATE_DEGRADED)) {
        return NUN_ESTATE;
    }

    if (kloz_budget_charge_work(&runtime->budget, 1u) != 0) {
        if (runtime->state == NUN_STATE_RUNNING) {
            (void)reaper_runtime_transition(&runtime->bootstrap,
                                            REAPER_RUNTIME_EVENT_DEGRADE,
                                            NUN_EBUDGET);
        }
        runtime->state = NUN_STATE_DEGRADED;
        return NUN_EBUDGET;
    }

    if (nun_read_tick(runtime, &now) != NUN_OK) {
        (void)reaper_runtime_transition(&runtime->bootstrap,
                                        REAPER_RUNTIME_EVENT_FAULT,
                                        NUN_EPLATFORM);
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EPLATFORM;
    }

    result = co_runtime_poll(&runtime->lifecycle);
    if (result != CO_OK) {
        (void)reaper_runtime_transition(&runtime->bootstrap,
                                        REAPER_RUNTIME_EVENT_FAULT,
                                        result);
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EIO;
    }

    if (saintsi_heartbeat(&runtime->assurance,
                       NUN_HEALTH_COMPONENT_ID, now) != SAINTSI_OK ||
        saintsi_health_check(&runtime->assurance, now,
                          &health_state, &stale_count) != SAINTSI_OK) {
        (void)reaper_runtime_transition(&runtime->bootstrap,
                                        REAPER_RUNTIME_EVENT_FAULT,
                                        NUN_EHEALTH);
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EHEALTH;
    }

    if (health_state == SAINTSI_DEGRADED || stale_count != 0u) {
        if (runtime->state == NUN_STATE_RUNNING) {
            (void)reaper_runtime_transition(&runtime->bootstrap,
                                            REAPER_RUNTIME_EVENT_DEGRADE,
                                            NUN_EHEALTH);
        }
        runtime->state = NUN_STATE_DEGRADED;
        return NUN_EHEALTH;
    }

    if (runtime->state == NUN_STATE_DEGRADED) {
        if (reaper_runtime_transition(&runtime->bootstrap,
                                      REAPER_RUNTIME_EVENT_RECOVER,
                                      REAPER_OK) != REAPER_OK) {
            runtime->state = NUN_STATE_FAULTED;
            return NUN_EBOOT;
        }
    }

    runtime->state = NUN_STATE_RUNNING;
    return NUN_OK;
}

nun_result_t nun_runtime_stop(nun_runtime_t *runtime)
{
    int32_t result;

    if (runtime == NULL || runtime->initialized == 0u) return NUN_ESTATE;
    if (runtime->state == NUN_STATE_STOPPED) return NUN_OK;
    if (!reaper_runtime_is_serviceable(&runtime->bootstrap)) return NUN_ESTATE;

    result = reaper_runtime_shutdown(&runtime->bootstrap,
                                     runtime->boot_steps, 4u);
    if (result != REAPER_OK) {
        runtime->state = NUN_STATE_FAULTED;
        return NUN_EIO;
    }

    runtime->initialized = 0u;
    runtime->state = NUN_STATE_STOPPED;
    return NUN_OK;
}

void *nun_runtime_allocate(nun_runtime_t *runtime, size_t bytes,
                           size_t alignment)
{
    void *allocation;

    if (runtime == NULL || runtime->initialized == 0u || bytes == 0u) {
        return NULL;
    }
    if (kloz_budget_reserve_memory(&runtime->budget, bytes) != 0) {
        return NULL;
    }

    allocation = kloz_arena_alloc(&runtime->arena, bytes, alignment);
    if (allocation == NULL) {
        (void)kloz_budget_release_memory(&runtime->budget, bytes);
        return NULL;
    }
    /* Account requested payload, not alignment padding in the arena. */
    if (bytes > SIZE_MAX - runtime->arena_reserved_bytes) {
        (void)kloz_budget_release_memory(&runtime->budget, bytes);
        return NULL;
    }
    runtime->arena_reserved_bytes += bytes;
    return allocation;
}

nun_result_t nun_runtime_reset_arena(nun_runtime_t *runtime)
{
    if (runtime == NULL || runtime->initialized == 0u) return NUN_ESTATE;
    if (runtime->arena_reserved_bytes > 0u &&
        kloz_budget_release_memory(&runtime->budget,
                                   runtime->arena_reserved_bytes) != 0) {
        return NUN_EBUDGET;
    }
    kloz_arena_reset(&runtime->arena);
    runtime->arena_reserved_bytes = 0u;
    return NUN_OK;
}

nun_result_t nun_runtime_reset_work_budget(nun_runtime_t *runtime)
{
    if (runtime == NULL || runtime->initialized == 0u) return NUN_ESTATE;
    return kloz_budget_reset_work(&runtime->budget) == 0
        ? NUN_OK : NUN_EBUDGET;
}

nun_result_t nun_runtime_reserve_queue_slots(nun_runtime_t *runtime,
                                             uint32_t slots)
{
    if (runtime == NULL || runtime->initialized == 0u || slots == 0u) {
        return NUN_EINVAL;
    }
    return kloz_budget_reserve_slots(&runtime->budget, slots) == 0
        ? NUN_OK : NUN_EBUDGET;
}

nun_result_t nun_runtime_release_queue_slots(nun_runtime_t *runtime,
                                             uint32_t slots)
{
    if (runtime == NULL || runtime->initialized == 0u || slots == 0u) {
        return NUN_EINVAL;
    }
    return kloz_budget_release_slots(&runtime->budget, slots) == 0
        ? NUN_OK : NUN_EBUDGET;
}

int nun_runtime_authorize(nun_access_operation_t operation,
                          nun_access_origin_t origin)
{
    return wdep_authorize((WdepOperation)operation, (WdepOrigin)origin);
}

nun_state_t nun_runtime_state(const nun_runtime_t *runtime)
{
    return runtime == NULL ? NUN_STATE_FAULTED : runtime->state;
}

nun_result_t nun_runtime_health(const nun_runtime_t *runtime,
                                saintsi_health_state_t *state_out,
                                uint32_t *stale_count_out)
{
    uint64_t ticks = 0u;
    uint32_t now;

    if (runtime == NULL || runtime->initialized == 0u ||
        state_out == NULL || stale_count_out == NULL) {
        return NUN_EINVAL;
    }
    if (runtime->platform.ticks_read(runtime->platform.context,
                                     &ticks) != KLOZ_OK) {
        return NUN_EPLATFORM;
    }
    now = (uint32_t)ticks;
    return saintsi_health_check(&runtime->assurance, now,
                             state_out, stale_count_out) == SAINTSI_OK
        ? NUN_OK : NUN_EHEALTH;
}
