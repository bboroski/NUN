#include "nun/scheduler_policy.h"
#include <string.h>

static nun_scheduler_task_t *find_task(nun_scheduler_t *s, uint32_t id,
                                       uint32_t *slot_out)
{
    uint32_t i;
    for (i = 0u; i < NUN_SCHEDULER_MAX_TASKS; ++i) {
        if (s->tasks[i].state != (uint8_t)NUN_TASK_FREE && s->tasks[i].id == id) {
            if (slot_out != NULL) *slot_out = i;
            return &s->tasks[i];
        }
    }
    return NULL;
}
static int observe_time(nun_scheduler_t *s, uint64_t now)
{
    if (s->has_time != 0u && now < s->last_now) return 0;
    s->last_now = now;
    s->has_time = 1u;
    return 1;
}
static uint8_t effective_priority(const nun_scheduler_t *s,
                                  const nun_scheduler_task_t *task, uint64_t now)
{
    uint64_t boost = 0u, priority;
    if (s->aging_interval_ticks != 0u && now > task->ready_since)
        boost = (now - task->ready_since) / s->aging_interval_ticks;
    priority = (uint64_t)task->base_priority + boost;
    if (priority >= (uint64_t)NUN_SCHEDULER_PRIORITY_LEVELS)
        priority = (uint64_t)NUN_SCHEDULER_PRIORITY_LEVELS - 1u;
    return (uint8_t)priority;
}
static void fill_decision(const nun_scheduler_t *s, uint32_t slot, uint64_t now,
                          nun_schedule_decision_t *out)
{
    const nun_scheduler_task_t *task = &s->tasks[slot];
    uint64_t elapsed = now - task->last_run_at;
    out->task_id = task->id;
    out->slot = slot;
    out->run_count = task->run_count;
    out->base_priority = task->base_priority;
    out->effective_priority = effective_priority(s, task, now);
    out->reserved = 0u;
    out->remaining_quantum_ticks = elapsed >= s->quantum_ticks
        ? 0u : s->quantum_ticks - elapsed;
}
nun_scheduler_result_t nun_scheduler_init(nun_scheduler_t *s, uint64_t quantum,
                                           uint64_t aging)
{
    if (s == NULL || quantum == 0u) return NUN_SCHEDULER_EINVAL;
    memset(s, 0, sizeof(*s));
    s->quantum_ticks = quantum;
    s->aging_interval_ticks = aging;
    s->current_slot = NUN_SCHEDULER_NO_SLOT;
    s->initialized = 1u;
    return NUN_SCHEDULER_OK;
}
nun_scheduler_result_t nun_scheduler_add_task(nun_scheduler_t *s, uint32_t id,
    uint8_t priority, uint64_t now)
{
    uint32_t i;
    nun_scheduler_task_t *task;
    if (s == NULL || id == 0u ||
        priority >= (uint8_t)NUN_SCHEDULER_PRIORITY_LEVELS)
        return NUN_SCHEDULER_EINVAL;
    if (s->initialized == 0u) return NUN_SCHEDULER_ESTATE;
    if (find_task(s, id, NULL) != NULL) return NUN_SCHEDULER_EDUPLICATE;
    if (s->task_count >= NUN_SCHEDULER_MAX_TASKS) return NUN_SCHEDULER_EFULL;
    if (!observe_time(s, now)) return NUN_SCHEDULER_ECLOCK;
    for (i = 0u; i < NUN_SCHEDULER_MAX_TASKS; ++i) {
        if (s->tasks[i].state == (uint8_t)NUN_TASK_FREE) {
            task = &s->tasks[i];
            memset(task, 0, sizeof(*task));
            task->id = id;
            task->base_priority = priority;
            task->ready_since = now;
            task->state = (uint8_t)NUN_TASK_READY;
            ++s->task_count;
            return NUN_SCHEDULER_OK;
        }
    }
    return NUN_SCHEDULER_EFULL;
}
nun_scheduler_result_t nun_scheduler_pick_next(nun_scheduler_t *s, uint64_t now,
                                                nun_schedule_decision_t *out)
{
    uint32_t offset, best = NUN_SCHEDULER_NO_SLOT;
    uint8_t best_priority = 0u;
    if (s == NULL || out == NULL) return NUN_SCHEDULER_EINVAL;
    if (s->initialized == 0u) return NUN_SCHEDULER_ESTATE;
    if (!observe_time(s, now)) return NUN_SCHEDULER_ECLOCK;
    if (s->current_slot != NUN_SCHEDULER_NO_SLOT) {
        nun_scheduler_task_t *current = &s->tasks[s->current_slot];
        uint64_t elapsed = now - current->last_run_at;
        if (current->state == (uint8_t)NUN_TASK_RUNNING &&
            elapsed < s->quantum_ticks) {
            fill_decision(s, s->current_slot, now, out);
            return NUN_SCHEDULER_OK;
        }
        if (current->state == (uint8_t)NUN_TASK_RUNNING) {
            current->state = (uint8_t)NUN_TASK_READY;
            current->ready_since = now;
        }
        s->current_slot = NUN_SCHEDULER_NO_SLOT;
    }
    for (offset = 0u; offset < NUN_SCHEDULER_MAX_TASKS; ++offset) {
        uint32_t slot = (s->cursor + offset) % NUN_SCHEDULER_MAX_TASKS;
        const nun_scheduler_task_t *candidate = &s->tasks[slot];
        uint8_t priority;
        if (candidate->state != (uint8_t)NUN_TASK_READY) continue;
        priority = effective_priority(s, candidate, now);
        if (best == NUN_SCHEDULER_NO_SLOT || priority > best_priority) {
            best = slot;
            best_priority = priority;
        }
    }
    if (best == NUN_SCHEDULER_NO_SLOT) return NUN_SCHEDULER_ENO_RUNNABLE;
    s->tasks[best].state = (uint8_t)NUN_TASK_RUNNING;
    s->tasks[best].last_run_at = now;
    if (s->tasks[best].run_count != UINT64_MAX) ++s->tasks[best].run_count;
    if (s->dispatch_count != UINT64_MAX) ++s->dispatch_count;
    s->current_slot = best;
    s->cursor = (best + 1u) % NUN_SCHEDULER_MAX_TASKS;
    fill_decision(s, best, now, out);
    return NUN_SCHEDULER_OK;
}
static nun_scheduler_result_t change_current(nun_scheduler_t *s, uint32_t id,
    uint64_t now, nun_scheduler_task_state_t next)
{
    uint32_t slot;
    nun_scheduler_task_t *task;
    if (s == NULL || id == 0u) return NUN_SCHEDULER_EINVAL;
    if (s->initialized == 0u) return NUN_SCHEDULER_ESTATE;
    task = find_task(s, id, &slot);
    if (task == NULL) return NUN_SCHEDULER_ENOTFOUND;
    if (s->current_slot != slot || task->state != (uint8_t)NUN_TASK_RUNNING)
        return NUN_SCHEDULER_ESTATE;
    if (!observe_time(s, now)) return NUN_SCHEDULER_ECLOCK;
    task->state = (uint8_t)next;
    if (next == NUN_TASK_READY) task->ready_since = now;
    s->current_slot = NUN_SCHEDULER_NO_SLOT;
    return NUN_SCHEDULER_OK;
}
nun_scheduler_result_t nun_scheduler_yield(nun_scheduler_t *s, uint32_t id,
                                             uint64_t now)
{ return change_current(s, id, now, NUN_TASK_READY); }
nun_scheduler_result_t nun_scheduler_block(nun_scheduler_t *s, uint32_t id,
                                              uint64_t now)
{ return change_current(s, id, now, NUN_TASK_BLOCKED); }
nun_scheduler_result_t nun_scheduler_wake(nun_scheduler_t *s, uint32_t id,
                                             uint64_t now)
{
    nun_scheduler_task_t *task;
    if (s == NULL || id == 0u) return NUN_SCHEDULER_EINVAL;
    if (s->initialized == 0u) return NUN_SCHEDULER_ESTATE;
    task = find_task(s, id, NULL);
    if (task == NULL) return NUN_SCHEDULER_ENOTFOUND;
    if (task->state != (uint8_t)NUN_TASK_BLOCKED) return NUN_SCHEDULER_ESTATE;
    if (!observe_time(s, now)) return NUN_SCHEDULER_ECLOCK;
    task->state = (uint8_t)NUN_TASK_READY;
    task->ready_since = now;
    return NUN_SCHEDULER_OK;
}
nun_scheduler_result_t nun_scheduler_terminate(nun_scheduler_t *s, uint32_t id,
                                                 uint64_t now)
{
    uint32_t slot;
    nun_scheduler_task_t *task;
    if (s == NULL || id == 0u) return NUN_SCHEDULER_EINVAL;
    if (s->initialized == 0u) return NUN_SCHEDULER_ESTATE;
    task = find_task(s, id, &slot);
    if (task == NULL) return NUN_SCHEDULER_ENOTFOUND;
    if (!observe_time(s, now)) return NUN_SCHEDULER_ECLOCK;
    if (s->current_slot == slot) s->current_slot = NUN_SCHEDULER_NO_SLOT;
    memset(task, 0, sizeof(*task));
    --s->task_count;
    return NUN_SCHEDULER_OK;
}
nun_scheduler_result_t nun_scheduler_query_task(const nun_scheduler_t *s,
    uint32_t id, nun_scheduler_task_state_t *state_out)
{
    const nun_scheduler_task_t *task;
    if (s == NULL || id == 0u || state_out == NULL) return NUN_SCHEDULER_EINVAL;
    if (s->initialized == 0u) return NUN_SCHEDULER_ESTATE;
    task = find_task((nun_scheduler_t *)s, id, NULL);
    if (task == NULL) return NUN_SCHEDULER_ENOTFOUND;
    *state_out = (nun_scheduler_task_state_t)task->state;
    return NUN_SCHEDULER_OK;
}
