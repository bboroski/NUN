#include "nun/runtime_advice.h"

#include <string.h>

nun_result_t nun_runtime_get_advice(const nun_runtime_t *runtime,
                                    nun_runtime_advice_t *advice)
{
    if (runtime == NULL || advice == NULL) return NUN_EINVAL;
    if (runtime->budget.memory_used > runtime->budget.memory_limit ||
        runtime->budget.queue_used > runtime->budget.queue_limit ||
        runtime->budget.work_used > runtime->budget.work_limit) return NUN_EBUDGET;
    memset(advice, 0, sizeof(*advice));
    advice->initialized = runtime->initialized;
    advice->memory_available = runtime->budget.memory_limit - runtime->budget.memory_used;
    advice->queue_available = runtime->budget.queue_limit - runtime->budget.queue_used;
    advice->work_available = runtime->budget.work_limit - runtime->budget.work_used;
    advice->rollback_failed_mask = runtime->bootstrap.rollback_failed_mask;
    advice->last_boot_error = runtime->bootstrap.last_error;
    advice->health_state = SAINTSI_DEGRADED;
    if (runtime->initialized != 0u &&
        nun_runtime_health(runtime, &advice->health_state,
                           &advice->stale_components) != NUN_OK) {
        advice->requires_attention = 1u;
    }
    switch (runtime->state) {
    case NUN_STATE_COLD:
        advice->state_key = "cold";
        advice->next_action_key = "validate_platform_and_initialize_runtime";
        break;
    case NUN_STATE_READY:
        advice->state_key = "ready";
        advice->next_action_key = "start_application_lifecycle";
        advice->can_start = 1u;
        break;
    case NUN_STATE_RUNNING:
        advice->state_key = "running";
        advice->next_action_key = "poll_one_bounded_runtime_cycle";
        advice->can_poll = 1u;
        advice->can_stop = 1u;
        break;
    case NUN_STATE_DEGRADED:
        advice->state_key = "degraded";
        advice->next_action_key = advice->stale_components != 0u
            ? "inspect_stale_component_heartbeats"
            : "review_resource_budget_and_recent_runtime_result";
        advice->can_stop = 1u;
        advice->requires_attention = 1u;
        break;
    case NUN_STATE_STOPPED:
        advice->state_key = "stopped";
        advice->next_action_key = "reinitialize_before_next_start";
        break;
    case NUN_STATE_FAULTED:
        advice->state_key = "faulted";
        advice->next_action_key = runtime->bootstrap.rollback_failed_mask != 0u
            ? "recover_resources_marked_by_rollback_failed_mask"
            : "inspect_last_error_and_boot_stage_before_reinitializing";
        advice->requires_attention = 1u;
        break;
    default:
        advice->state_key = "invalid";
        advice->next_action_key = "reinitialize_runtime_after_external_review";
        advice->requires_attention = 1u;
        break;
    }
    if (runtime->bootstrap.rollback_failed_mask != 0u) {
        advice->requires_attention = 1u;
        advice->next_action_key = "recover_resources_marked_by_rollback_failed_mask";
    }
    return NUN_OK;
}
