#ifndef NUN_SCHEDULER_POLICY_H
#define NUN_SCHEDULER_POLICY_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define NUN_SCHEDULER_POLICY_ABI_VERSION UINT32_C(0x00010000)
#define NUN_SCHEDULER_MAX_TASKS UINT32_C(64)
#define NUN_SCHEDULER_PRIORITY_LEVELS UINT32_C(8)
#define NUN_SCHEDULER_NO_SLOT UINT32_MAX
typedef enum nun_scheduler_result {
    NUN_SCHEDULER_OK = 0, NUN_SCHEDULER_EINVAL = -1,
    NUN_SCHEDULER_ESTATE = -2, NUN_SCHEDULER_EFULL = -3,
    NUN_SCHEDULER_EDUPLICATE = -4, NUN_SCHEDULER_ENOTFOUND = -5,
    NUN_SCHEDULER_ENO_RUNNABLE = -6, NUN_SCHEDULER_ECLOCK = -7
} nun_scheduler_result_t;
typedef enum nun_scheduler_task_state {
    NUN_TASK_FREE = 0, NUN_TASK_READY = 1,
    NUN_TASK_RUNNING = 2, NUN_TASK_BLOCKED = 3
} nun_scheduler_task_state_t;
typedef struct nun_scheduler_task {
    uint32_t id;
    uint64_t ready_since, last_run_at, run_count;
    uint8_t base_priority, state;
    uint16_t reserved;
} nun_scheduler_task_t;
typedef struct nun_scheduler {
    nun_scheduler_task_t tasks[NUN_SCHEDULER_MAX_TASKS];
    uint64_t quantum_ticks, aging_interval_ticks, last_now, dispatch_count;
    uint32_t task_count, cursor, current_slot;
    uint8_t has_time, initialized, reserved[6];
} nun_scheduler_t;
typedef struct nun_schedule_decision {
    uint32_t task_id, slot;
    uint64_t run_count, remaining_quantum_ticks;
    uint8_t base_priority, effective_priority;
    uint16_t reserved;
} nun_schedule_decision_t;
/* Policy only: no register/stack switching, user mode, or preemption. */
nun_scheduler_result_t nun_scheduler_init(nun_scheduler_t *scheduler,
    uint64_t quantum_ticks, uint64_t aging_interval_ticks);
nun_scheduler_result_t nun_scheduler_add_task(nun_scheduler_t *scheduler,
    uint32_t task_id, uint8_t priority, uint64_t now_ticks);
nun_scheduler_result_t nun_scheduler_pick_next(nun_scheduler_t *scheduler,
    uint64_t now_ticks, nun_schedule_decision_t *out);
nun_scheduler_result_t nun_scheduler_yield(nun_scheduler_t *scheduler,
    uint32_t task_id, uint64_t now_ticks);
nun_scheduler_result_t nun_scheduler_block(nun_scheduler_t *scheduler,
    uint32_t task_id, uint64_t now_ticks);
nun_scheduler_result_t nun_scheduler_wake(nun_scheduler_t *scheduler,
    uint32_t task_id, uint64_t now_ticks);
nun_scheduler_result_t nun_scheduler_terminate(nun_scheduler_t *scheduler,
    uint32_t task_id, uint64_t now_ticks);
nun_scheduler_result_t nun_scheduler_query_task(const nun_scheduler_t *scheduler,
    uint32_t task_id, nun_scheduler_task_state_t *state_out);
#ifdef __cplusplus
}
#endif
#endif
