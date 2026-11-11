#ifndef NUN_RUNTIME_ADVICE_H
#define NUN_RUNTIME_ADVICE_H
#include "nun/runtime.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct nun_runtime_advice {
    const char *state_key;
    const char *next_action_key;
    size_t memory_available;
    uint32_t queue_available;
    uint64_t work_available;
    uint32_t stale_components;
    saintsi_health_state_t health_state;
    uint8_t initialized;
    uint8_t can_start;
    uint8_t can_poll;
    uint8_t can_stop;
    uint8_t requires_attention;
    uint32_t rollback_failed_mask;
    int32_t last_boot_error;
} nun_runtime_advice_t;
/* Read-only runtime/resource/health summary for an operator or dashboard. */
nun_result_t nun_runtime_get_advice(const nun_runtime_t *runtime,
                                    nun_runtime_advice_t *advice);
#ifdef __cplusplus
}
#endif
#endif
