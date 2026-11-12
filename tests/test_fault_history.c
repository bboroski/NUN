#include "nun/fault_history.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
int main(void)
{
    nun_fault_history_t ledger;
    nun_fault_event_t rows[NUN_FAULT_CAPACITY];
    size_t i, count;
    nun_fault_history_init(&ledger);
    assert(nun_fault_history_append(&ledger, 10u, 1u, 7u, 1u, 900u, NUN_FAULT_FLAG_OBSERVED, 0u) == 1);
    assert(nun_fault_history_append(&ledger, 11u, 2u, 7u, 2u, 800u,
        NUN_FAULT_FLAG_OBSERVED | NUN_FAULT_FLAG_CORROBORATED, 1u) == 1);
    assert(nun_fault_history_append(&ledger, 9u, 3u, 7u, 1u, 500u, NUN_FAULT_FLAG_OBSERVED, 0u) == 0);
    assert(nun_fault_history_append(&ledger, 12u, 3u, 7u, 1u, 1001u, NUN_FAULT_FLAG_OBSERVED, 0u) == 0);
    assert(nun_fault_history_append(&ledger, 12u, 3u, 7u, 1u, 500u, NUN_FAULT_FLAG_CORROBORATED, 0u) == 0);
    assert(nun_fault_history_append(&ledger, 12u, 3u, 7u, 1u, 500u, NUN_FAULT_FLAG_OBSERVED, ledger.next_sequence) == 0);
    assert(nun_fault_history_snapshot(&ledger, NULL, 0u) == 2u);
    count = nun_fault_history_snapshot(&ledger, rows, NUN_FAULT_CAPACITY);
    assert(count == 2u && rows[0].sequence == 1u && rows[1].sequence == 2u);
    for (i = 0u; i < NUN_FAULT_CAPACITY + 3u; ++i)
        assert(nun_fault_history_append(&ledger, 20u + (uint64_t)i, 10u + (uint32_t)i,
            7u, 0u, 1000u, NUN_FAULT_FLAG_OBSERVED, 0u) == 1);
    assert(ledger.count == NUN_FAULT_CAPACITY && ledger.dropped == 5u);
    count = nun_fault_history_snapshot(&ledger, rows, NUN_FAULT_CAPACITY);
    assert(count == NUN_FAULT_CAPACITY && rows[0].sequence == 6u);
    puts("bounded event history tests passed");
    return 0;
}
