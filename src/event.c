#include "nun/event.h"

#include <string.h>

static int nun_event_text_valid(const char *text, size_t capacity, int required)
{
    if (text == NULL || memchr(text, '\0', capacity) == NULL) {
        return 0;
    }
    return required == 0 || text[0] != '\0';
}

nun_event_result_t nun_event_validate_v1(const nun_event_v1_t *event)
{
    if (event == NULL) {
        return NUN_EVENT_E_ARGUMENT;
    }
    if (event->abi_version != NUN_EVENT_ABI_VERSION) {
        return NUN_EVENT_E_VERSION;
    }
    if (event->struct_size != (uint32_t)sizeof(*event)) {
        return NUN_EVENT_E_SIZE;
    }
    if (!nun_event_text_valid(event->event_id,
                              sizeof(event->event_id), 1) ||
        !nun_event_text_valid(event->producer,
                              sizeof(event->producer), 1) ||
        !nun_event_text_valid(event->kind,
                              sizeof(event->kind), 1) ||
        !nun_event_text_valid(event->correlation_id,
                              sizeof(event->correlation_id), 0)) {
        return NUN_EVENT_E_TEXT;
    }
    if (event->severity > (uint32_t)NUN_EVENT_CRITICAL) {
        return NUN_EVENT_E_SEVERITY;
    }
    if (event->data_classification >
        (uint32_t)NUN_EVENT_RESTRICTED) {
        return NUN_EVENT_E_CLASSIFICATION;
    }
    if (event->payload == NULL ||
        event->payload_size < 2U ||
        event->payload_size > (size_t)NUN_EVENT_MAX_PAYLOAD_BYTES) {
        return NUN_EVENT_E_PAYLOAD;
    }

    return NUN_EVENT_OK;
}
