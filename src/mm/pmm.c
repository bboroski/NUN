#include "nun/pmm.h"

#include <limits.h>
#include <stdint.h>

static const struct limine_memmap_response *g_memory_map;
static uint64_t g_hhdm_offset;
static size_t g_entry_index;
static uint64_t g_next_physical;
static uint64_t g_usable_bytes;
static uint64_t g_allocated_pages;

static uint64_t align_up_page(uint64_t value)
{
    const uint64_t mask = NUN_PAGE_SIZE - 1u;
    if (value > UINT64_MAX - mask) return 0u;
    return (value + mask) & ~mask;
}

bool nun_pmm_init(const struct limine_memmap_response *memory_map,
                  uint64_t hhdm_offset)
{
    uint64_t total = 0u;
    uint64_t i;
    if (memory_map == 0 || memory_map->entries == 0 ||
        memory_map->entry_count == 0u || memory_map->entry_count > 4096u) {
        return false;
    }

    for (i = 0u; i < memory_map->entry_count; ++i) {
        const struct limine_memmap_entry *entry = memory_map->entries[i];
        if (entry == 0 || entry->base > UINT64_MAX - entry->length) return false;
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            if (entry->length > UINT64_MAX - total) return false;
            total += entry->length;
        }
    }

    g_memory_map = memory_map;
    g_hhdm_offset = hhdm_offset;
    g_entry_index = 0u;
    g_next_physical = 0u;
    g_usable_bytes = total;
    g_allocated_pages = 0u;
    return total >= NUN_PAGE_SIZE;
}

void *nun_pmm_alloc_pages(size_t page_count)
{
    uint64_t bytes, i;
    if (g_memory_map == 0 || page_count == 0u ||
        (uint64_t)page_count > UINT64_MAX / NUN_PAGE_SIZE ||
        (uint64_t)page_count > UINT64_MAX - g_allocated_pages) return 0;
    bytes = (uint64_t)page_count * NUN_PAGE_SIZE;

    for (i = (uint64_t)g_entry_index; i < g_memory_map->entry_count; ++i) {
        const struct limine_memmap_entry *entry = g_memory_map->entries[i];
        uint64_t end, start, physical, virtual_address, j;
        volatile uint8_t *memory;

        if (entry == 0 || entry->type != LIMINE_MEMMAP_USABLE ||
            entry->length == 0u) continue;
        if (entry->base > UINT64_MAX - entry->length) continue;
        end = (entry->base + entry->length) & ~(NUN_PAGE_SIZE - 1u);
        start = (i == (uint64_t)g_entry_index && g_next_physical != 0u)
            ? g_next_physical : align_up_page(entry->base);
        if (start == 0u || start > end || bytes > end - start) {
            g_next_physical = 0u;
            g_entry_index = (size_t)i + 1u;
            continue;
        }

        physical = start;
        if (physical > UINT64_MAX - g_hhdm_offset) return 0;
        virtual_address = physical + g_hhdm_offset;
        if (virtual_address > (uint64_t)UINTPTR_MAX ||
            bytes - 1u > (uint64_t)UINTPTR_MAX - virtual_address) return 0;

        memory = (volatile uint8_t *)(uintptr_t)virtual_address;
        for (j = 0u; j < bytes; ++j) memory[j] = 0u;

        g_next_physical = physical + bytes;
        g_entry_index = (size_t)i;
        g_allocated_pages += (uint64_t)page_count;
        return (void *)(uintptr_t)virtual_address;
    }
    return 0;
}

uint64_t nun_pmm_usable_bytes(void) { return g_usable_bytes; }
uint64_t nun_pmm_allocated_pages(void) { return g_allocated_pages; }
