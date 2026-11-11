#include "nun/runtime_advice.h"
#include <assert.h>
#include <string.h>
typedef struct { uint64_t ticks; } test_context_t;
static kloz_status_t ticks(void *context, uint64_t *out)
{
    test_context_t *state=(test_context_t *)context;
    if(!state||!out)return KLOZ_EINVAL;
    *out=++state->ticks;return KLOZ_OK;
}
static co_result_t init_ok(void *p){(void)p;return CO_OK;}
static co_result_t step_ok(void *p){(void)p;return CO_OK;}
int main(void)
{
    test_context_t context={0};
    unsigned char arena[256];
    nun_runtime_t runtime;
    nun_runtime_advice_t advice;
    co_hooks_t hooks={&context,init_ok,step_ok,0};
    kloz_platform_ops_t platform={KLOZ_ABI_VERSION,(uint32_t)sizeof(kloz_platform_ops_t),KLOZ_PLATFORM_CAP_MONOTONIC_CLOCK,&context,ticks,0,0,0};
    nun_runtime_config_t config={.platform=&platform,.lifecycle_hooks=&hooks,.arena_storage=arena,.arena_capacity=sizeof(arena),.memory_limit=128u,.queue_limit=4u,.work_limit=12u,.heartbeat_timeout_ticks=20u};
    assert(nun_runtime_get_advice(0,&advice)==NUN_EINVAL);
    assert(nun_runtime_init(&runtime,&config)==NUN_OK);
    assert(nun_runtime_get_advice(&runtime,&advice)==NUN_OK);
    assert(strcmp(advice.state_key,"ready")==0&&advice.can_start==1u);
    assert(advice.memory_available==128u&&advice.work_available==12u);
    assert(nun_runtime_start(&runtime)==NUN_OK);
    assert(nun_runtime_get_advice(&runtime,&advice)==NUN_OK);
    assert(strcmp(advice.state_key,"running")==0&&advice.can_poll==1u);
    assert(nun_runtime_allocate(&runtime,32u,8u)!=0);
    assert(nun_runtime_get_advice(&runtime,&advice)==NUN_OK);
    assert(advice.memory_available==96u);
    runtime.bootstrap.rollback_failed_mask=2u;
    assert(nun_runtime_get_advice(&runtime,&advice)==NUN_OK);
    assert(advice.requires_attention==1u);
    assert(strcmp(advice.next_action_key,"recover_resources_marked_by_rollback_failed_mask")==0);
    runtime.bootstrap.rollback_failed_mask=0u;
    assert(nun_runtime_stop(&runtime)==NUN_OK);
    assert(nun_runtime_get_advice(&runtime,&advice)==NUN_OK);
    assert(strcmp(advice.state_key,"stopped")==0);
    return 0;
}
