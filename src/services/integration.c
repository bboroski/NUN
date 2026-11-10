#include "nun/services.h"

#include <stdint.h>

#include "pbsc/arena.h"
#include "pbsc/platform.h"
#include "kloz/budget.h"
#include "dlo/runtime.h"
#include "riae/health.h"
#include "riae/crc32.h"
#include "reaper_runtime.h"

static uint8_t g_service_arena_storage[2048] __attribute__((aligned(16)));
static pbsc_arena_t g_service_arena;
static kloz_budget_t g_service_budget;
static dlo_runtime_t g_component_runtime;
static riae_supervisor_t g_health_supervisor;
static reaper_runtime_t g_boot_supervisor;
static uint32_t g_component_steps;

static dlo_result_t component_init(void *context)
{
    (void)context;
    g_component_steps = 0u;
    return DLO_OK;
}

static dlo_result_t component_step(void *context)
{
    (void)context;
    if (g_component_steps < UINT32_MAX) g_component_steps++;
    return DLO_OK;
}

static void component_shutdown(void *context)
{
    (void)context;
}

bool nun_services_smoke_init(void)
{
    dlo_hooks_t hooks = {
        .context = 0,
        .init = component_init,
        .step = component_step,
        .shutdown = component_shutdown
    };
    uint8_t *arena_probe;
    riae_health_state_t health_state = RIAE_DEGRADED;
    uint32_t stale_count = 1u;
    uint32_t crc = 0u;
    static const uint8_t crc_probe[] = { 'N', 'U', 'N', 0u };

    /* Kloz: caller-owned arena and explicit resource reservations. */
    pbsc_arena_init(&g_service_arena, g_service_arena_storage,
                    sizeof(g_service_arena_storage));
    arena_probe = (uint8_t *)pbsc_arena_alloc(&g_service_arena, 64u, 16u);
    if (arena_probe == 0 || pbsc_arena_used(&g_service_arena) < 64u)
        return false;
    if (pbsc_platform_validate(0) != PBSC_EINVAL)
        return false;

    if (kloz_budget_init(&g_service_budget, 1024u, 4u, 10000u) != KLOZ_OK)
        return false;
    if (kloz_budget_reserve_memory(&g_service_budget, 64u) != KLOZ_OK)
        return false;
    if (kloz_budget_reserve_slots(&g_service_budget, 1u) != KLOZ_OK)
        return false;
    if (kloz_budget_charge_work(&g_service_budget, 1u) != KLOZ_OK)
        return false;
    if (kloz_budget_release_slots(&g_service_budget, 1u) != KLOZ_OK)
        return false;
    if (kloz_budget_release_memory(&g_service_budget, 64u) != KLOZ_OK)
        return false;

    /* CO: one deterministic lifecycle step, with no thread or hidden allocator. */
    if (dlo_runtime_init(&g_component_runtime, &hooks) != DLO_OK ||
        dlo_runtime_start(&g_component_runtime) != DLO_OK ||
        dlo_runtime_poll(&g_component_runtime) != DLO_OK ||
        dlo_runtime_stop(&g_component_runtime) != DLO_OK ||
        g_component_steps != 1u)
        return false;

    /* SaintSI: observe a registered component; CRC32 is only an error check. */
    riae_supervisor_init(&g_health_supervisor);
    if (riae_register_component(&g_health_supervisor, 1u, 100u, 10u) != RIAE_OK ||
        riae_heartbeat(&g_health_supervisor, 1u, 11u) != RIAE_OK ||
        riae_health_check(&g_health_supervisor, 11u,
                          &health_state, &stale_count) != RIAE_OK ||
        health_state != RIAE_HEALTHY || stale_count != 0u ||
        riae_crc32(crc_probe, sizeof(crc_probe), &crc) != RIAE_OK)
        return false;
    if (crc == 0u) return false;

    /* ReaperCripple: validate its deterministic boot-state transition contract.
       No board profile, MMIO, clock programming or actuator is invoked here. */
    if (reaper_runtime_init(&g_boot_supervisor) != REAPER_OK ||
        reaper_runtime_transition(&g_boot_supervisor,
            REAPER_RUNTIME_EVENT_BEGIN_BOOT, REAPER_OK) != REAPER_OK ||
        reaper_runtime_transition(&g_boot_supervisor,
            REAPER_RUNTIME_EVENT_BOOT_COMPLETE, REAPER_OK) != REAPER_OK ||
        reaper_runtime_transition(&g_boot_supervisor,
            REAPER_RUNTIME_EVENT_START, REAPER_OK) != REAPER_OK ||
        g_boot_supervisor.state != REAPER_RUNTIME_RUNNING)
        return false;

    return true;
}
