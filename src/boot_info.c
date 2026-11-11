#include "nun/boot_info.h"

#include <limits.h>

static int nun_architecture_is_known(uint32_t architecture)
{
    return architecture == (uint32_t)NUN_ARCH_X86_64 ||
           architecture == (uint32_t)NUN_ARCH_AARCH64 ||
           architecture == (uint32_t)NUN_ARCH_RISCV64 ||
           architecture == (uint32_t)NUN_ARCH_E2K;
}

static int nun_memory_type_is_known(uint32_t type)
{
    return type >= (uint32_t)NUN_MEMORY_AVAILABLE &&
           type <= (uint32_t)NUN_MEMORY_MMIO;
}

static int nun_ranges_overlap(uint64_t a_start,
                              uint64_t a_end,
                              uint64_t b_start,
                              uint64_t b_end)
{
    return a_start < b_end && b_start < a_end;
}

nun_boot_result_t nun_boot_info_validate(
    const nun_boot_info_t *info,
    const nun_memory_region_t *regions,
    size_t region_count)
{
    uint64_t kernel_end;
    size_t i;
    size_t j;
    int kernel_region_found = 0;

    if (info == NULL || regions == NULL) {
        return NUN_BOOT_E_ARGUMENT;
    }
    if (info->magic != NUN_BOOT_INFO_MAGIC) {
        return NUN_BOOT_E_MAGIC;
    }
    if (info->version != NUN_BOOT_INFO_VERSION) {
        return NUN_BOOT_E_VERSION;
    }
    if (info->header_size != (uint16_t)sizeof(*info) ||
        info->total_size != (uint32_t)sizeof(*info)) {
        return NUN_BOOT_E_SIZE;
    }
    if (!nun_architecture_is_known(info->architecture)) {
        return NUN_BOOT_E_ARCHITECTURE;
    }
    if (info->flags != 0U) {
        return NUN_BOOT_E_FLAGS;
    }
    if (region_count == 0U ||
        region_count > NUN_BOOT_INFO_MAX_REGIONS ||
        region_count != (size_t)info->memory_map_count ||
        info->memory_map_address == 0U) {
        return NUN_BOOT_E_MEMORY_MAP;
    }
    if (info->kernel_image_size == 0U ||
        info->kernel_load_address > UINT64_MAX - info->kernel_image_size) {
        return NUN_BOOT_E_RANGE;
    }

    kernel_end = info->kernel_load_address + info->kernel_image_size;
    if (info->kernel_entry_address < info->kernel_load_address ||
        info->kernel_entry_address >= kernel_end) {
        return NUN_BOOT_E_RANGE;
    }

    for (i = 0U; i < region_count; ++i) {
        uint64_t region_end;
        const nun_memory_region_t *region = &regions[i];

        if (region->length == 0U ||
            region->base > UINT64_MAX - region->length ||
            !nun_memory_type_is_known(region->type)) {
            return NUN_BOOT_E_MEMORY_MAP;
        }
        if (region->flags != 0U) {
            return NUN_BOOT_E_MEMORY_MAP;
        }
        region_end = region->base + region->length;

        for (j = 0U; j < i; ++j) {
            const uint64_t other_end = regions[j].base + regions[j].length;
            if (nun_ranges_overlap(region->base, region_end,
                                   regions[j].base, other_end)) {
                return NUN_BOOT_E_MEMORY_MAP;
            }
        }

        if (region->type == (uint32_t)NUN_MEMORY_KERNEL &&
            info->kernel_load_address >= region->base &&
            kernel_end <= region_end) {
            kernel_region_found = 1;
        }
    }

    return kernel_region_found ? NUN_BOOT_OK : NUN_BOOT_E_KERNEL_REGION;
}
