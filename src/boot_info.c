#include "nun/boot_info.h"

#include <stdint.h>

static int nun_architecture_supported(uint32_t architecture)
{
    return architecture == (uint32_t)NUN_ARCH_X86_64 ||
           architecture == (uint32_t)NUN_ARCH_AARCH64 ||
           architecture == (uint32_t)NUN_ARCH_RISCV64;
}

static int nun_memory_type_valid(uint32_t type)
{
    return type >= (uint32_t)NUN_MEMORY_USABLE &&
           type <= (uint32_t)NUN_MEMORY_FIRMWARE;
}

static int nun_add_overflows_u64(uint64_t base, uint64_t length)
{
    return length > UINT64_MAX - base;
}

static int nun_page_aligned(uint64_t value)
{
    return (value % NUN_BOOT_PAGE_SIZE) == 0u;
}

nun_status_t nun_boot_info_validate(
    const nun_boot_info_t *boot_info,
    const nun_memory_region_t *mapped_memory_map,
    size_t mapped_entry_count)
{
    size_t index;
    uint64_t previous_end = 0u;

    if (boot_info == NULL || mapped_memory_map == NULL) {
        return NUN_STATUS_INVALID_ARGUMENT;
    }

    if (boot_info->magic != NUN_BOOT_INFO_MAGIC) {
        return NUN_STATUS_BAD_MAGIC;
    }

    if (boot_info->struct_size < (uint32_t)sizeof(nun_boot_info_t) ||
        boot_info->abi_major != NUN_BOOT_INFO_ABI_MAJOR ||
        boot_info->abi_minor > NUN_BOOT_INFO_ABI_MINOR) {
        return NUN_STATUS_UNSUPPORTED_ABI;
    }

    if (!nun_architecture_supported(boot_info->architecture)) {
        return NUN_STATUS_UNSUPPORTED_ARCHITECTURE;
    }

    if (boot_info->flags != 0u) {
        return NUN_STATUS_RESERVED_FLAGS;
    }

    if (boot_info->memory_map_physical_address == 0u ||
        (boot_info->memory_map_physical_address % 8u) != 0u ||
        boot_info->memory_map_count == 0u ||
        boot_info->memory_map_count > NUN_BOOT_INFO_MAX_MEMORY_MAP_ENTRIES ||
        mapped_entry_count != (size_t)boot_info->memory_map_count ||
        boot_info->memory_map_entry_size != (uint32_t)sizeof(nun_memory_region_t) ||
        ((uintptr_t)mapped_memory_map % _Alignof(nun_memory_region_t)) != 0u) {
        return NUN_STATUS_INVALID_MEMORY_MAP;
    }

    for (index = 0u; index < mapped_entry_count; ++index) {
        const nun_memory_region_t *region = &mapped_memory_map[index];
        uint64_t region_end;

        if (region->length == 0u ||
            !nun_page_aligned(region->base) ||
            !nun_page_aligned(region->length) ||
            !nun_memory_type_valid(region->type) ||
            nun_add_overflows_u64(region->base, region->length)) {
            return NUN_STATUS_INVALID_MEMORY_MAP;
        }

        region_end = region->base + region->length;
        if (index > 0u && region->base < previous_end) {
            return NUN_STATUS_MEMORY_MAP_OVERLAP;
        }
        previous_end = region_end;
    }

    if (boot_info->kernel_image_size == 0u ||
        !nun_page_aligned(boot_info->kernel_physical_base) ||
        nun_add_overflows_u64(boot_info->kernel_physical_base,
                              boot_info->kernel_image_size)) {
        return NUN_STATUS_INVALID_RANGE;
    }

    if (boot_info->initrd_size == 0u) {
        if (boot_info->initrd_physical_base != 0u) {
            return NUN_STATUS_INVALID_RANGE;
        }
    } else if (boot_info->initrd_physical_base == 0u ||
               !nun_page_aligned(boot_info->initrd_physical_base) ||
               !nun_page_aligned(boot_info->initrd_size) ||
               nun_add_overflows_u64(boot_info->initrd_physical_base,
                                     boot_info->initrd_size)) {
        return NUN_STATUS_INVALID_RANGE;
    }

    if (boot_info->framebuffer_physical_address == 0u) {
        if (boot_info->framebuffer_width != 0u ||
            boot_info->framebuffer_height != 0u ||
            boot_info->framebuffer_pitch != 0u ||
            boot_info->framebuffer_format != 0u) {
            return NUN_STATUS_INVALID_FRAMEBUFFER;
        }
    } else if (boot_info->framebuffer_width == 0u ||
               boot_info->framebuffer_height == 0u ||
               boot_info->framebuffer_pitch == 0u ||
               boot_info->framebuffer_format == 0u) {
        return NUN_STATUS_INVALID_FRAMEBUFFER;
    }

    return NUN_STATUS_OK;
}

const char *nun_status_name(nun_status_t status)
{
    switch (status) {
    case NUN_STATUS_OK:
        return "ok";
    case NUN_STATUS_INVALID_ARGUMENT:
        return "invalid-argument";
    case NUN_STATUS_BAD_MAGIC:
        return "bad-magic";
    case NUN_STATUS_UNSUPPORTED_ABI:
        return "unsupported-abi";
    case NUN_STATUS_UNSUPPORTED_ARCHITECTURE:
        return "unsupported-architecture";
    case NUN_STATUS_INVALID_MEMORY_MAP:
        return "invalid-memory-map";
    case NUN_STATUS_MEMORY_MAP_OVERLAP:
        return "memory-map-overlap";
    case NUN_STATUS_INVALID_RANGE:
        return "invalid-range";
    case NUN_STATUS_INVALID_FRAMEBUFFER:
        return "invalid-framebuffer";
    case NUN_STATUS_RESERVED_FLAGS:
        return "reserved-flags-set";
    default:
        return "unknown-status";
    }
}
