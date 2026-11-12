#ifndef NUN_FAULT_HISTORY_H
#define NUN_FAULT_HISTORY_H
#include <stddef.h>
#include <stdint.h>

/* Fixed-capacity event history. No allocation, I/O, clocks or payload capture. */
#define NUN_FAULT_CAPACITY 64u
#define NUN_FAULT_FLAG_OBSERVED UINT8_C(0x01)
#define NUN_FAULT_FLAG_CORROBORATED UINT8_C(0x02)
#define NUN_FAULT_FLAG_INFERRED UINT8_C(0x04)
#define NUN_FAULT_FLAG_EXTERNAL UINT8_C(0x08)
#define NUN_FAULT_FLAG_MASK UINT8_C(0x0F)

typedef struct {
    uint64_t sequence;
    uint64_t monotonic_ticks;
    uint64_t related_sequence;
    uint32_t event_code;
    uint32_t source_code;
    uint16_t confidence_permille;
    uint8_t severity;
    uint8_t evidence_flags;
} nun_fault_event_t;

typedef struct {
    nun_fault_event_t records[NUN_FAULT_CAPACITY];
    size_t head, count;
    uint64_t next_sequence;
    uint32_t dropped;
} nun_fault_history_t;

static inline void nun_fault_history_init(nun_fault_history_t *ledger)
{
    size_t i;
    if (ledger == NULL) return;
    ledger->head = 0u; ledger->count = 0u;
    ledger->next_sequence = UINT64_C(1); ledger->dropped = 0u;
    for (i = 0u; i < NUN_FAULT_CAPACITY; ++i) {
        ledger->records[i].sequence = 0u;
        ledger->records[i].monotonic_ticks = 0u;
        ledger->records[i].related_sequence = 0u;
        ledger->records[i].event_code = 0u;
        ledger->records[i].source_code = 0u;
        ledger->records[i].confidence_permille = 0u;
        ledger->records[i].severity = 0u;
        ledger->records[i].evidence_flags = 0u;
    }
}

/* Confidence is an evidence annotation, never a truth probability. */
static inline int nun_fault_history_append(
    nun_fault_history_t *ledger, uint64_t monotonic_ticks,
    uint32_t event_code, uint32_t source_code, uint8_t severity,
    uint16_t confidence_permille, uint8_t evidence_flags,
    uint64_t related_sequence)
{
    size_t slot;
    nun_fault_event_t record;
    if (ledger == NULL || event_code == 0u || source_code == 0u ||
        severity > 4u || confidence_permille > 1000u ||
        evidence_flags == 0u || (evidence_flags & (uint8_t)~NUN_FAULT_FLAG_MASK) != 0u ||
        ((evidence_flags & NUN_FAULT_FLAG_CORROBORATED) != 0u &&
         (evidence_flags & NUN_FAULT_FLAG_OBSERVED) == 0u) ||
        ledger->next_sequence == UINT64_MAX ||
        (ledger->count != 0u &&
         monotonic_ticks < ledger->records[(ledger->head + ledger->count - 1u) % NUN_FAULT_CAPACITY].monotonic_ticks) ||
        (related_sequence != 0u && related_sequence >= ledger->next_sequence)) return 0;
    record.sequence = ledger->next_sequence;
    record.monotonic_ticks = monotonic_ticks;
    record.related_sequence = related_sequence;
    record.event_code = event_code;
    record.source_code = source_code;
    record.confidence_permille = confidence_permille;
    record.severity = severity;
    record.evidence_flags = evidence_flags;
    if (ledger->count == NUN_FAULT_CAPACITY) {
        slot = ledger->head;
        ledger->head = (ledger->head + 1u) % NUN_FAULT_CAPACITY;
        if (ledger->dropped != UINT32_MAX) ++ledger->dropped;
    } else {
        slot = (ledger->head + ledger->count) % NUN_FAULT_CAPACITY;
        ++ledger->count;
    }
    ledger->records[slot] = record;
    ++ledger->next_sequence;
    return 1;
}

static inline size_t nun_fault_history_snapshot(const nun_fault_history_t *ledger, nun_fault_event_t *out, size_t capacity)
{
    size_t i;
    if (ledger == NULL) return 0u;
    if (out == NULL || capacity < ledger->count) return ledger->count;
    for (i = 0u; i < ledger->count; ++i)
        out[i] = ledger->records[(ledger->head + i) % NUN_FAULT_CAPACITY];
    return ledger->count;
}
#endif
