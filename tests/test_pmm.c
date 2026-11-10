#include "nun/pmm.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t page_pool[8192] __attribute__((aligned(4096)));

int main(void)
{
    struct limine_memmap_entry entry;
    struct limine_memmap_entry *entries[1];
    struct limine_memmap_response map;

    entry.base = (uint64_t)(uintptr_t)page_pool;
    entry.length = sizeof(page_pool);
    entry.type = LIMINE_MEMMAP_USABLE;
    entries[0] = &entry;
    map.revision = 0u;
    map.entry_count = 1u;
    map.entries = entries;

    assert(nun_pmm_init(&map, 0u));
    assert(nun_pmm_usable_bytes() == sizeof(page_pool));
    assert(nun_pmm_allocated_pages() == 0u);

    for (size_t i = 0u; i < sizeof(page_pool); ++i) page_pool[i] = 0xA5u;

    void *first = nun_pmm_alloc_pages(1u);
    assert(first == page_pool);
    for (size_t i = 0u; i < 4096u; ++i)
        assert(((uint8_t *)first)[i] == 0u);

    void *second = nun_pmm_alloc_pages(1u);
    assert(second == (void *)(page_pool + 4096u));
    assert(nun_pmm_alloc_pages(1u) == 0);
    assert(nun_pmm_allocated_pages() == 2u);
    assert(nun_pmm_alloc_pages(0u) == 0);

    puts("NUN PMM tests: PASS");
    return 0;
}
