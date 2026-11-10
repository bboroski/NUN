#ifndef NUN_PMM_H
#define NUN_PMM_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "limine.h"
#define NUN_PAGE_SIZE UINT64_C(4096)
bool nun_pmm_init(const struct limine_memmap_response *memory_map, uint64_t hhdm_offset);
void *nun_pmm_alloc_pages(size_t page_count);
uint64_t nun_pmm_usable_bytes(void);
uint64_t nun_pmm_allocated_pages(void);
#endif
