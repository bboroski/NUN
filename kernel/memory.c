#include "nun/memory.h"
#include "nun/serial.h"

#define MULTIBOOT2_TAG_END 0u
#define MULTIBOOT2_TAG_MODULE 3u
#define MULTIBOOT2_TAG_MEMORY_MAP 6u
#define MULTIBOOT2_MEMORY_AVAILABLE 1u
#define NUN_IDENTITY_LIMIT (1ull << 30)

typedef struct __attribute__((packed)) {
    uint32_t type;
    uint32_t size;
} mb2_tag_t;

typedef struct __attribute__((packed)) {
    mb2_tag_t header;
    uint32_t entry_size;
    uint32_t entry_version;
} mb2_mmap_tag_t;

typedef struct __attribute__((packed)) {
    uint64_t address;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} mb2_mmap_entry_t;

extern uint8_t __kernel_end[];

static const mb2_mmap_entry_t *g_memory_entries;
static uint32_t g_memory_entry_size;
static uint32_t g_memory_entry_count;
static uint64_t g_usable_bytes;
static uint64_t g_next_candidate;

static uint64_t align_up_page(uint64_t value)
{
    return (value + (uint64_t)NUN_PAGE_SIZE - 1u) & ~((uint64_t)NUN_PAGE_SIZE - 1u);
}

int nun_memory_init(uintptr_t info_address)
{
    const uint8_t *info;
    uint32_t total_size;
    uint32_t offset;
    uint64_t reserved_end = (uint64_t)(uintptr_t)__kernel_end;
    int found_map = 0;

    g_memory_entries = NULL;
    g_memory_entry_size = 0u;
    g_memory_entry_count = 0u;
    g_usable_bytes = 0u;
    g_next_candidate = 0u;

    if (info_address == 0u || (info_address & 7u) != 0u) {
        return 0;
    }

    info = (const uint8_t *)info_address;
    total_size = *(const uint32_t *)info;
    if (total_size < 16u || (uint64_t)info_address + total_size > NUN_IDENTITY_LIMIT) {
        return 0;
    }
    if ((uint64_t)info_address + total_size > reserved_end) {
        reserved_end = (uint64_t)info_address + total_size;
    }

    offset = 8u;
    while (offset + (uint32_t)sizeof(mb2_tag_t) <= total_size) {
        const mb2_tag_t *tag = (const mb2_tag_t *)(const void *)(info + offset);
        uint32_t next_offset;
        if (tag->size < 8u || tag->size > total_size - offset) {
            return 0;
        }
        if (tag->type == MULTIBOOT2_TAG_MEMORY_MAP) {
            const mb2_mmap_tag_t *mmap = (const mb2_mmap_tag_t *)(const void *)tag;
            if (tag->size < 16u || mmap->entry_size < (uint32_t)sizeof(mb2_mmap_entry_t) ||
                ((tag->size - 16u) % mmap->entry_size) != 0u) {
                return 0;
            }
            g_memory_entries = (const mb2_mmap_entry_t *)(const void *)(info + offset + 16u);
            g_memory_entry_size = mmap->entry_size;
            g_memory_entry_count = (tag->size - 16u) / mmap->entry_size;
            found_map = 1;
        } else if (tag->type == MULTIBOOT2_TAG_MODULE && tag->size >= 16u) {
            const uint32_t *module = (const uint32_t *)(const void *)(info + offset + 8u);
            if ((uint64_t)module[1] > reserved_end) {
                reserved_end = module[1];
            }
        }
        if (tag->type == MULTIBOOT2_TAG_END) {
            break;
        }
        next_offset = (offset + tag->size + 7u) & ~7u;
        if (next_offset <= offset) {
            return 0;
        }
        offset = next_offset;
    }

    if (found_map == 0 || g_memory_entries == NULL || g_memory_entry_count == 0u) {
        return 0;
    }

    for (uint32_t index = 0u; index < g_memory_entry_count; ++index) {
        const mb2_mmap_entry_t *entry = (const mb2_mmap_entry_t *)
            ((const uint8_t *)(const void *)g_memory_entries + (uint64_t)index * g_memory_entry_size);
        uint64_t end;
        uint64_t start;
        if (entry->type != MULTIBOOT2_MEMORY_AVAILABLE || entry->length == 0u ||
            entry->address > UINT64_MAX - entry->length) {
            continue;
        }
        end = entry->address + entry->length;
        if (end > NUN_IDENTITY_LIMIT) {
            end = NUN_IDENTITY_LIMIT;
        }
        start = entry->address;
        if (start < reserved_end) {
            start = reserved_end;
        }
        if (start < end) {
            g_usable_bytes += end - start;
        }
    }

    g_next_candidate = align_up_page(reserved_end);
    return 1;
}

uint64_t nun_memory_usable_bytes(void)
{
    return g_usable_bytes;
}

void *nun_memory_allocate_frame(void)
{
    if (g_memory_entries == NULL) {
        return NULL;
    }

    for (uint32_t index = 0u; index < g_memory_entry_count; ++index) {
        const mb2_mmap_entry_t *entry = (const mb2_mmap_entry_t *)
            ((const uint8_t *)(const void *)g_memory_entries + (uint64_t)index * g_memory_entry_size);
        uint64_t end;
        uint64_t start;
        volatile uint64_t *words;
        if (entry->type != MULTIBOOT2_MEMORY_AVAILABLE || entry->length == 0u ||
            entry->address > UINT64_MAX - entry->length) {
            continue;
        }
        end = entry->address + entry->length;
        if (end > NUN_IDENTITY_LIMIT) {
            end = NUN_IDENTITY_LIMIT;
        }
        start = entry->address > g_next_candidate ? entry->address : g_next_candidate;
        start = align_up_page(start);
        if (start > end || end - start < (uint64_t)NUN_PAGE_SIZE) {
            continue;
        }
        g_next_candidate = start + (uint64_t)NUN_PAGE_SIZE;
        words = (volatile uint64_t *)(uintptr_t)start;
        for (uint32_t word = 0u; word < NUN_PAGE_SIZE / sizeof(uint64_t); ++word) {
            words[word] = 0u;
        }
        return (void *)(uintptr_t)start;
    }
    return NULL;
}
