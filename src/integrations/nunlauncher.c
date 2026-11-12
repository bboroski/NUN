#include "nun/integrations/nunlauncher.h"

static int map_architecture(nun_architecture_t source,
                            nunl_architecture_t *destination)
{
    if (destination == NULL) {
        return 0;
    }
    switch (source) {
    case NUN_ARCH_X86_64:
        *destination = NUNL_ARCH_X86_64;
        return 1;
    case NUN_ARCH_AARCH64:
        *destination = NUNL_ARCH_AARCH64;
        return 1;
    case NUN_ARCH_RISCV64:
        *destination = NUNL_ARCH_RISCV64;
        return 1;
    case NUN_ARCH_E2K:
        *destination = NUNL_ARCH_E2K;
        return 1;
    default:
        return 0;
    }
}

nunl_result_t nun_boot_admission_validate_image(
    const void *image,
    size_t available_bytes,
    uint64_t load_address,
    const nun_memory_region_t *regions,
    size_t region_count,
    nun_architecture_t target_architecture,
    uint64_t *entry_address_out)
{
    nunl_memory_range_t translated[NUNL_MAX_MEMORY_RANGES];
    nunl_architecture_t launcher_architecture;
    size_t index;

    if (image == NULL || regions == NULL || entry_address_out == NULL ||
        region_count == 0u || region_count > NUNL_MAX_MEMORY_RANGES) {
        return NUNL_EINVAL;
    }
    if (!map_architecture(target_architecture, &launcher_architecture)) {
        return NUNL_EARCH;
    }

    for (index = 0u; index < region_count; ++index) {
        translated[index].base = regions[index].base;
        translated[index].size = regions[index].length;
        translated[index].reserved = 0u;
        if (regions[index].flags != 0u) {
            return NUNL_EMEMORY;
        }
        switch (regions[index].type) {
        case NUN_MEMORY_AVAILABLE:
            translated[index].type = (uint32_t)NUNL_MEMORY_USABLE_RAM;
            break;
        case NUN_MEMORY_RESERVED:
        case NUN_MEMORY_KERNEL:
            translated[index].type = (uint32_t)NUNL_MEMORY_RESERVED;
            break;
        case NUN_MEMORY_MMIO:
            translated[index].type = (uint32_t)NUNL_MEMORY_MMIO;
            break;
        default:
            return NUNL_EMEMORY;
        }
    }

    return nunl_validate_image(image, available_bytes, load_address,
                               translated, region_count,
                               launcher_architecture, entry_address_out);
}
