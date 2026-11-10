#include "nun/boot_info.h"

#include <stddef.h>

static int architecture_known(uint32_t architecture)
{
    return architecture == NUN_ARCH_X86_64 ||
           architecture == NUN_ARCH_AARCH64 ||
           architecture == NUN_ARCH_RISCV64 ||
           architecture == NUN_ARCH_E2K;
}

static int region_type_known(uint32_t type)
{
    return type == NUN_MEMORY_AVAILABLE ||
           type == NUN_MEMORY_LOADED_KERNEL ||
           type == NUN_MEMORY_RESERVED ||
           type == NUN_MEMORY_FRAMEBUFFER;
}

static int range_end(uint64_t base, uint64_t length, uint64_t *end_out)
{
    if (length == 0U || end_out == NULL || UINT64_MAX - base < length) {
        return 0;
    }
    *end_out = base + length;
    return 1;
}

nun_boot_status_t nun_boot_info_validate(const nun_boot_info_t *info)
{
    uint64_t kernel_end;
    uint32_t i;
    uint32_t kernel_regions = 0U;
    int entry_in_kernel_region = 0;

    if (info == NULL) {
        return NUN_BOOT_EINVAL;
    }
    if (info->magic != NUN_BOOT_INFO_MAGIC ||
        info->version != NUN_BOOT_INFO_VERSION ||
        info->struct_size < (uint32_t)sizeof(*info) ||
        info->flags != 0U) {
        return NUN_BOOT_EVERSION;
    }
    if (!architecture_known(info->architecture)) {
        return NUN_BOOT_EARCH;
    }
    if (info->memory_regions == NULL ||
        info->memory_region_count == 0U ||
        info->memory_region_count > NUN_BOOT_MAX_MEMORY_REGIONS) {
        return NUN_BOOT_EMEMMAP;
    }
    if (!range_end(info->kernel_base, info->kernel_size, &kernel_end)) {
        return NUN_BOOT_ERANGE;
    }
    if (info->entry_address < info->kernel_base ||
        info->entry_address >= kernel_end) {
        return NUN_BOOT_EENTRY;
    }

    for (i = 0U; i < info->memory_region_count; ++i) {
        const nun_memory_region_t *region = &info->memory_regions[i];
        uint64_t region_end;
        uint32_t j;

        if (region->reserved != 0U || !region_type_known(region->type) ||
            !range_end(region->base, region->length, &region_end)) {
            return NUN_BOOT_EMEMMAP;
        }

        if (region->type == NUN_MEMORY_LOADED_KERNEL &&
            region->base <= info->kernel_base &&
            region_end >= kernel_end) {
            ++kernel_regions;
        }

        if (region->type == NUN_MEMORY_LOADED_KERNEL &&
            info->entry_address >= region->base &&
            info->entry_address < region_end) {
            entry_in_kernel_region = 1;
        }

        for (j = i + 1U; j < info->memory_region_count; ++j) {
            const nun_memory_region_t *other = &info->memory_regions[j];
            uint64_t other_end;
            if (other->reserved != 0U || !region_type_known(other->type) ||
                !range_end(other->base, other->length, &other_end)) {
                return NUN_BOOT_EMEMMAP;
            }
            if (region->base < other_end && other->base < region_end) {
                return NUN_BOOT_EOVERLAP;
            }
        }
    }

    if (kernel_regions != 1U || !entry_in_kernel_region) {
        return NUN_BOOT_EENTRY;
    }
    return NUN_BOOT_OK;
}
