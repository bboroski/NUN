#ifndef NUN_EVENT_H
#define NUN_EVENT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUN_EVENT_ABI_VERSION UINT32_C(0x00010000)
#define NUN_EVENT_TEXT_CAPACITY 64U
#define NUN_EVENT_MAX_PAYLOAD_BYTES 4096U

typedef enum nun_event_result {
    NUN_EVENT_OK = 0,
    NUN_EVENT_E_ARGUMENT = -1,
    NUN_EVENT_E_VERSION = -2,
    NUN_EVENT_E_SIZE = -3,
    NUN_EVENT_E_TEXT = -4,
    NUN_EVENT_E_SEVERITY = -5,
    NUN_EVENT_E_CLASSIFICATION = -6,
    NUN_EVENT_E_PAYLOAD = -7
} nun_event_result_t;

typedef enum nun_event_severity {
    NUN_EVENT_TRACE = 0,
    NUN_EVENT_INFO = 1,
    NUN_EVENT_WARNING = 2,
    NUN_EVENT_ERROR = 3,
    NUN_EVENT_CRITICAL = 4
} nun_event_severity_t;

typedef enum nun_event_classification {
    NUN_EVENT_PUBLIC = 0,
    NUN_EVENT_INTERNAL = 1,
    NUN_EVENT_SENSITIVE = 2,
    NUN_EVENT_RESTRICTED = 3
} nun_event_classification_t;

/*
 * Version 1 event envelope.
 *
 * Text fields are fixed-capacity, NUL-terminated UTF-8 strings. event_id,
 * producer and kind are required; correlation_id may be empty. sequence is
 * monotonic per producer, not globally ordered. monotonic_ticks must come
 * from the producer's monotonic clock domain.
 *
 * payload is a caller-owned, serialized JSON object. The validator checks
 * pointer and size bounds but deliberately does not parse JSON or copy data.
 * The payload must remain valid for the duration of its consumer's use.
 */
typedef struct nun_event_v1 {
    uint32_t abi_version;
    uint32_t struct_size;
    char event_id[NUN_EVENT_TEXT_CAPACITY];
    char producer[NUN_EVENT_TEXT_CAPACITY];
    uint64_t sequence;
    uint64_t monotonic_ticks;
    char kind[NUN_EVENT_TEXT_CAPACITY];
    char correlation_id[NUN_EVENT_TEXT_CAPACITY];
    uint32_t severity;
    uint32_t data_classification;
    const uint8_t *payload;
    size_t payload_size;
} nun_event_v1_t;

/* Pure validation: no allocation, I/O, queue mutation or authorization. */
nun_event_result_t nun_event_validate_v1(const nun_event_v1_t *event);

#ifdef __cplusplus
}
#endif

#endif
