#include "nun/event.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void make_valid(nun_event_v1_t *event)
{
    static const uint8_t payload[] = "{}";

    memset(event, 0, sizeof(*event));
    event->abi_version = NUN_EVENT_ABI_VERSION;
    event->struct_size = (uint32_t)sizeof(*event);
    (void)strcpy(event->event_id, "evt-0001");
    (void)strcpy(event->producer, "nun.runtime");
    event->sequence = UINT64_C(1);
    event->monotonic_ticks = UINT64_C(42);
    (void)strcpy(event->kind, "runtime.ready");
    event->severity = (uint32_t)NUN_EVENT_INFO;
    event->data_classification = (uint32_t)NUN_EVENT_INTERNAL;
    event->payload = payload;
    event->payload_size = sizeof(payload) - 1U;
}

int main(void)
{
    nun_event_v1_t event;

    make_valid(&event);
    assert(nun_event_validate_v1(&event) == NUN_EVENT_OK);
    assert(nun_event_validate_v1(NULL) == NUN_EVENT_E_ARGUMENT);

    event.abi_version++;
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_VERSION);
    make_valid(&event);

    event.struct_size--;
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_SIZE);
    make_valid(&event);

    event.event_id[0] = '\0';
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_TEXT);
    make_valid(&event);

    memset(event.producer, 'x', sizeof(event.producer));
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_TEXT);
    make_valid(&event);

    event.kind[0] = '\0';
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_TEXT);
    make_valid(&event);

    event.severity = UINT32_C(99);
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_SEVERITY);
    make_valid(&event);

    event.data_classification = UINT32_C(99);
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_CLASSIFICATION);
    make_valid(&event);

    event.payload = NULL;
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_PAYLOAD);
    make_valid(&event);

    event.payload_size = 1U;
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_PAYLOAD);
    make_valid(&event);

    event.payload_size = (size_t)NUN_EVENT_MAX_PAYLOAD_BYTES + 1U;
    assert(nun_event_validate_v1(&event) == NUN_EVENT_E_PAYLOAD);
    make_valid(&event);

    /* An empty correlation ID is valid and means "not correlated". */
    assert(event.correlation_id[0] == '\0');
    assert(nun_event_validate_v1(&event) == NUN_EVENT_OK);

    puts("NUN event envelope v1 tests: PASS");
    return 0;
}
