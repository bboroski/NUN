#include "nun/scheduler_policy.h"
#include <assert.h>
#include <stdint.h>
static void priority_quantum(void)
{
    nun_scheduler_t s; nun_schedule_decision_t d;
    assert(nun_scheduler_init(&s, 2u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 10u, 1u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 20u, 6u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 20u, 6u, 0u) == NUN_SCHEDULER_EDUPLICATE);
    assert(nun_scheduler_pick_next(&s, 0u, &d) == NUN_SCHEDULER_OK);
    assert(d.task_id == 20u && d.remaining_quantum_ticks == 2u);
    assert(nun_scheduler_pick_next(&s, 1u, &d) == NUN_SCHEDULER_OK);
    assert(d.task_id == 20u && d.remaining_quantum_ticks == 1u);
    assert(nun_scheduler_pick_next(&s, 2u, &d) == NUN_SCHEDULER_OK);
    assert(d.task_id == 20u && d.run_count == 2u);
}
static void round_robin_yield(void)
{
    nun_scheduler_t s; nun_schedule_decision_t d;
    assert(nun_scheduler_init(&s, 2u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 1u, 3u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 2u, 3u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_pick_next(&s, 0u, &d) == NUN_SCHEDULER_OK && d.task_id == 1u);
    assert(nun_scheduler_pick_next(&s, 2u, &d) == NUN_SCHEDULER_OK && d.task_id == 2u);
    assert(nun_scheduler_pick_next(&s, 4u, &d) == NUN_SCHEDULER_OK && d.task_id == 1u);
    assert(nun_scheduler_yield(&s, 1u, 5u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_pick_next(&s, 5u, &d) == NUN_SCHEDULER_OK && d.task_id == 2u);
}
static void block_wake_terminate(void)
{
    nun_scheduler_t s; nun_schedule_decision_t d; nun_scheduler_task_state_t state;
    assert(nun_scheduler_init(&s, 4u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 7u, 2u, 10u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_pick_next(&s, 10u, &d) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_block(&s, 7u, 11u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_pick_next(&s, 11u, &d) == NUN_SCHEDULER_ENO_RUNNABLE);
    assert(nun_scheduler_query_task(&s, 7u, &state) == NUN_SCHEDULER_OK);
    assert(state == NUN_TASK_BLOCKED);
    assert(nun_scheduler_wake(&s, 7u, 12u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_pick_next(&s, 12u, &d) == NUN_SCHEDULER_OK && d.task_id == 7u);
    assert(nun_scheduler_terminate(&s, 7u, 13u) == NUN_SCHEDULER_OK);
    assert(s.task_count == 0u);
    assert(nun_scheduler_query_task(&s, 7u, &state) == NUN_SCHEDULER_ENOTFOUND);
}
static void aging_and_clock_guard(void)
{
    nun_scheduler_t s; nun_schedule_decision_t d;
    assert(nun_scheduler_init(&s, 2u, 1u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 1u, 0u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 2u, 5u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_pick_next(&s, 0u, &d) == NUN_SCHEDULER_OK && d.task_id == 2u);
    assert(nun_scheduler_pick_next(&s, 7u, &d) == NUN_SCHEDULER_OK);
    assert(d.task_id == 1u && d.effective_priority == 7u);
    assert(nun_scheduler_pick_next(&s, 6u, &d) == NUN_SCHEDULER_ECLOCK);
}
static void invalid_transitions_and_inputs(void)
{
    nun_scheduler_t s;
    nun_schedule_decision_t d;
    nun_scheduler_task_state_t state;

    assert(nun_scheduler_init(&s, 3u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 0u, 1u, 0u) == NUN_SCHEDULER_EINVAL);
    assert(nun_scheduler_add_task(&s, 1u, 8u, 0u) == NUN_SCHEDULER_EINVAL);
    assert(nun_scheduler_wake(&s, 1u, 0u) == NUN_SCHEDULER_ENOTFOUND);
    assert(nun_scheduler_yield(&s, 1u, 0u) == NUN_SCHEDULER_ENOTFOUND);
    assert(nun_scheduler_pick_next(&s, 0u, &d) == NUN_SCHEDULER_ENO_RUNNABLE);

    assert(nun_scheduler_add_task(&s, 1u, 2u, 1u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_wake(&s, 1u, 1u) == NUN_SCHEDULER_ESTATE);
    assert(nun_scheduler_yield(&s, 1u, 1u) == NUN_SCHEDULER_ESTATE);
    assert(nun_scheduler_pick_next(&s, 1u, &d) == NUN_SCHEDULER_OK);
    assert(d.task_id == 1u);
    assert(nun_scheduler_wake(&s, 1u, 2u) == NUN_SCHEDULER_ESTATE);
    assert(nun_scheduler_query_task(&s, 1u, &state) == NUN_SCHEDULER_OK);
    assert(state == NUN_TASK_RUNNING);

    assert(nun_scheduler_terminate(&s, 1u, 0u) == NUN_SCHEDULER_ECLOCK);
    assert(nun_scheduler_query_task(&s, 1u, &state) == NUN_SCHEDULER_OK);
    assert(state == NUN_TASK_RUNNING);
    assert(nun_scheduler_terminate(&s, 1u, 2u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_pick_next(&s, 2u, &d) == NUN_SCHEDULER_ENO_RUNNABLE);
}
static void fixed_capacity(void)
{
    nun_scheduler_t s; nun_schedule_decision_t d; uint32_t i;
    assert(nun_scheduler_init(&s, 1u, 0u) == NUN_SCHEDULER_OK);
    for (i = 1u; i <= NUN_SCHEDULER_MAX_TASKS; ++i)
        assert(nun_scheduler_add_task(&s, i, 0u, 0u) == NUN_SCHEDULER_OK);
    assert(nun_scheduler_add_task(&s, 1000u, 0u, 0u) == NUN_SCHEDULER_EFULL);
    assert(nun_scheduler_pick_next(&s, 0u, &d) == NUN_SCHEDULER_OK);
    assert(d.task_id == 1u);
}
int main(void)
{
    nun_scheduler_t s;
    assert(nun_scheduler_init(NULL, 1u, 0u) == NUN_SCHEDULER_EINVAL);
    assert(nun_scheduler_init(&s, 0u, 0u) == NUN_SCHEDULER_EINVAL);
    priority_quantum(); round_robin_yield(); block_wake_terminate();
    aging_and_clock_guard(); invalid_transitions_and_inputs(); fixed_capacity();
    return 0;
}
