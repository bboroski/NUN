#include "nun/boot_info.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void make_valid(nun_boot_info_t *info,
                       nun_memory_region_t *regions,
                       size_t *region_count)
{
    memset(info, 0, sizeof(*info));
    info->magic = NUN_BOOT_INFO_MAGIC;
    info->version = NUN_BOOT_INFO_VERSION;
    info->header_size = (uint16_t)sizeof(*info);
    info->total_size = (uint32_t)sizeof(*info);
    info->architecture = (uint32_t)NUN_ARCH_X86_64;
    info->kernel_load_address = UINT64_C(0x100000);
    info->kernel_image_size = UINT64_C(0x20000);
    info->kernel_entry_address = UINT64_C(0x101000);
    info->memory_map_address = UINT64_C(0x8000);
    info->memory_map_count = 2U;

    regions[0].base = UINT64_C(0x100000);
    regions[0].length = UINT64_C(0x20000);
    regions[0].type = (uint32_t)NUN_MEMORY_KERNEL;
    regions[0].flags = 0U;
    regions[1].base = UINT64_C(0x120000);
    regions[1].length = UINT64_C(0x10000);
    regions[1].type = (uint32_t)NUN_MEMORY_AVAILABLE;
    regions[1].flags = 0U;
    *region_count = 2U;
}

int main(void)
{
    nun_boot_info_t info;
    nun_memory_region_t regions[NUN_BOOT_INFO_MAX_REGIONS];
    size_t count = 0U;

    make_valid(&info, regions, &count);
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_OK);

    info.magic = 0U;
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_MAGIC);
    make_valid(&info, regions, &count);

    info.version++;
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_VERSION);
    make_valid(&info, regions, &count);

    info.architecture = UINT32_C(99);
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_ARCHITECTURE);
    make_valid(&info, regions, &count);

    info.kernel_entry_address = UINT64_C(0x500000);
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_RANGE);
    make_valid(&info, regions, &count);

    regions[1].base = UINT64_C(0x110000);
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_MEMORY_MAP);
    make_valid(&info, regions, &count);

    regions[0].type = (uint32_t)NUN_MEMORY_RESERVED;
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_KERNEL_REGION);
    make_valid(&info, regions, &count);

    info.kernel_load_address = UINT64_MAX - UINT64_C(8);
    info.kernel_image_size = UINT64_C(16);
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_RANGE);
    make_valid(&info, regions, &count);

    info.memory_map_count = NUN_BOOT_INFO_MAX_REGIONS + 1U;
    assert(nun_boot_info_validate(&info, regions, count) == NUN_BOOT_E_MEMORY_MAP);

    puts("NUN boot-info contract tests: PASS");
    return 0;
}
