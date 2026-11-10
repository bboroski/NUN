#include "nun/boot_info.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    const nun_memory_region_t regions[] = {
        {0x100000U, 0x20000U, NUN_MEMORY_LOADED_KERNEL, 0U},
        {0x400000U, 0x10000U, NUN_MEMORY_AVAILABLE, 0U}
    };
    nun_boot_info_t info = {
        NUN_BOOT_INFO_MAGIC,
        NUN_BOOT_INFO_VERSION,
        (uint32_t)sizeof(nun_boot_info_t),
        NUN_ARCH_X86_64,
        0x100000U,
        0x20000U,
        0x100100U,
        regions,
        2U,
        0U
    };

    assert(nun_boot_info_validate(&info) == NUN_BOOT_OK);

    info.memory_regions = NULL;
    assert(nun_boot_info_validate(&info) == NUN_BOOT_EMEMMAP);
    info.memory_regions = regions;

    info.architecture = NUN_ARCH_UNKNOWN;
    assert(nun_boot_info_validate(&info) == NUN_BOOT_EARCH);
    info.architecture = NUN_ARCH_X86_64;

    info.kernel_base = UINT64_MAX - 8U;
    info.kernel_size = 32U;
    assert(nun_boot_info_validate(&info) == NUN_BOOT_ERANGE);
    info.kernel_base = 0x100000U;
    info.kernel_size = 0x20000U;

    info.entry_address = 0x300000U;
    assert(nun_boot_info_validate(&info) == NUN_BOOT_EENTRY);
    info.entry_address = 0x100100U;

    {
        const nun_memory_region_t overlapping[] = {
            {0x100000U, 0x20000U, NUN_MEMORY_LOADED_KERNEL, 0U},
            {0x110000U, 0x10000U, NUN_MEMORY_AVAILABLE, 0U}
        };
        info.memory_regions = overlapping;
        assert(nun_boot_info_validate(&info) == NUN_BOOT_EOVERLAP);
    }

    puts("NUN boot contract tests: PASS");
    return 0;
}
