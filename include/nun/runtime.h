#ifndef NUN_RUNTIME_H
#define NUN_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#include "pbsc/platform.h"
#include "dlo/runtime.h"
#include "riae/health.h"
#include "pbsc/arena.h"
#include "kloz/budget.h"
#include "reaper_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUN_RUNTIME_ABI_VERSION UINT32_C(0x00010000)
#define NUN_HEALTH_COMPONENT_ID UINT32_C(0x4E554E01)

typedef enum nun_result {
    NUN_OK = 0,
    NUN_EINVAL = -1,
    NUN_ESTATE = -2,
    NUN_EPLATFORM = -3,
    NUN_EBOOT = -4,
    NUN_EIO = -5,
    NUN_EBUDGET = -6,
    NUN_EHEALTH = -7
} nun_result_t;

typedef enum nun_state {
    NUN_STATE_COLD = 0,
    NUN_STATE_READY,
    NUN_STATE_RUNNING,
    NUN_STATE_DEGRADED,
    NUN_STATE_STOPPED,
    NUN_STATE_FAULTED
} nun_state_t;

typedef enum nun_access_operation {
    NUN_ACCESS_READ_LOCAL = 1,
    NUN_ACCESS_SCAN = 2,
    NUN_ACCESS_WRITE_PRIVATE_REPORT = 3,
    NUN_ACCESS_EXPORT = 4,
    NUN_ACCESS_CLIPBOARD = 5,
    NUN_ACCESS_MUTATE_DEVICE = 6,
    NUN_ACCESS_UPLOAD_DATA = 7,
    NUN_ACCESS_RECORD_LOCAL_PROFILE = 8
} nun_access_operation_t;

typedef enum nun_access_origin {
    NUN_ORIGIN_INTERNAL = 0,
    NUN_ORIGIN_USER = 1
} nun_access_origin_t;

typedef struct nun_runtime_config {
    const pbsc_platform_ops_t *platform;
    const dlo_hooks_t *lifecycle_hooks;
    void *arena_storage;
    size_t arena_capacity;
    size_t arena_reserved_bytes;
    size_t memory_limit;
    uint32_t queue_limit;
    uint64_t work_limit;
    uint32_t heartbeat_timeout_ticks;
} nun_runtime_config_t;

typedef struct nun_runtime {
    pbsc_platform_ops_t platform;
    dlo_hooks_t lifecycle_hooks;
    pbsc_arena_t arena;
    kloz_budget_t budget;
    dlo_runtime_t lifecycle;
    riae_supervisor_t assurance;
    reaper_runtime_t bootstrap;
    reaper_boot_step_t boot_steps[4];
    void *arena_storage;
    size_t arena_capacity;
    size_t memory_limit;
    uint32_t queue_limit;
    uint64_t work_limit;
    uint32_t heartbeat_timeout_ticks;
    uint32_t last_tick;
    nun_state_t state;
    uint8_t initialized;
} nun_runtime_t;

nun_result_t nun_runtime_init(nun_runtime_t *runtime,
                              const nun_runtime_config_t *config);
nun_result_t nun_runtime_start(nun_runtime_t *runtime);
nun_result_t nun_runtime_poll(nun_runtime_t *runtime);
nun_result_t nun_runtime_stop(nun_runtime_t *runtime);

void *nun_runtime_allocate(nun_runtime_t *runtime, size_t bytes,
                           size_t alignment);
nun_result_t nun_runtime_reset_arena(nun_runtime_t *runtime);
nun_result_t nun_runtime_reset_work_budget(nun_runtime_t *runtime);
nun_result_t nun_runtime_reserve_queue_slots(nun_runtime_t *runtime,
                                             uint32_t slots);
nun_result_t nun_runtime_release_queue_slots(nun_runtime_t *runtime,
                                             uint32_t slots);

/* Returns 1 when the ToolNG policy core allows the operation, otherwise 0. */
int nun_runtime_authorize(nun_access_operation_t operation,
                          nun_access_origin_t origin);

nun_state_t nun_runtime_state(const nun_runtime_t *runtime);
nun_result_t nun_runtime_health(const nun_runtime_t *runtime,
                                riae_health_state_t *state_out,
                                uint32_t *stale_count_out);

#ifdef __cplusplus
}
#endif

#endif
