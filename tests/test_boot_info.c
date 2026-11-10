#include "nun/boot_info.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static const nun_memory_region_t valid_map[] = {
    { UINT64_C(0x00100000), UINT64_C(0x00400000), NUN_MEMORY_USABLE, 0u },
    { UINT64_C(0x00500000), UINT64_C(0x00100000), NUN_MEMORY_RESERVED, 0u }
};

static nun_boot_info_t valid_boot_info(void)
{
    nun_boot_info_t info;
    memset(&info, 0, sizeof(info));
    info.magic = NUN_BOOT_INFO_MAGIC;
    info.struct_size = (uint32_t)sizeof(info);
    info.abi_major = NUN_BOOT_INFO_ABI_MAJOR;
    info.abi_minor = NUN_BOOT_INFO_ABI_MINOR;
    info.architecture = NUN_ARCH_X86_64;
    info.memory_map_physical_address = UINT64_C(0x00080000);
    info.memory_map_count = (uint32_t)(sizeof(valid_map) / sizeof(valid_map[0]));
    info.memory_map_entry_size = (uint32_t)sizeof(nun_memory_region_t);
    info.kernel_physical_base = UINT64_C(0x00100000);
    info.kernel_image_size = UINT64_C(0x00020000);
    return info;
}

static void test_valid_boot_info(void)
{
    nun_boot_info_t info = valid_boot_info();
    assert(nun_boot_info_validate(&info, valid_map, 2u) == NUN_STATUS_OK);
}

static void test_null_arguments(void)
{
    nun_boot_info_t info = valid_boot_info();
    assert(nun_boot_info_validate(NULL, valid_map, 2u) ==
           NUN_STATUS_INVALID_ARGUMENT);
    assert(nun_boot_info_validate(&info, NULL, 2u) ==
           NUN_STATUS_INVALID_ARGUMENT);
}

static void test_magic_and_abi(void)
{
    nun_boot_info_t info = valid_boot_info();
    info.magic = 0u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_BAD_MAGIC);

    info = valid_boot_info();
    info.abi_major = 99u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_UNSUPPORTED_ABI);

    info = valid_boot_info();
    info.struct_size = 1u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_UNSUPPORTED_ABI);
}

static void test_architecture_and_flags(void)
{
    nun_boot_info_t info = valid_boot_info();
    info.architecture = NUN_ARCH_UNKNOWN;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_UNSUPPORTED_ARCHITECTURE);

    info = valid_boot_info();
    info.flags = 1u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_RESERVED_FLAGS);
}

static void test_memory_map_shape(void)
{
    nun_boot_info_t info = valid_boot_info();
    info.memory_map_count = 0u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_INVALID_MEMORY_MAP);

    info = valid_boot_info();
    assert(nun_boot_info_validate(&info, valid_map, 1u) ==
           NUN_STATUS_INVALID_MEMORY_MAP);

    info = valid_boot_info();
    info.memory_map_entry_size = 1u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_INVALID_MEMORY_MAP);
}

static void test_invalid_and_overlapping_regions(void)
{
    nun_boot_info_t info = valid_boot_info();
    nun_memory_region_t map[2] = {
        { UINT64_C(0x00100000), UINT64_C(0x00200000), NUN_MEMORY_USABLE, 0u },
        { UINT64_C(0x00200000), UINT64_C(0x00100000), NUN_MEMORY_RESERVED, 0u }
    };
    assert(nun_boot_info_validate(&info, map, 2u) ==
           NUN_STATUS_MEMORY_MAP_OVERLAP);

    map[1].base = UINT64_C(0x00400000);
    map[1].length = 0u;
    assert(nun_boot_info_validate(&info, map, 2u) ==
           NUN_STATUS_INVALID_MEMORY_MAP);

    map[1].base = UINT64_C(0x00400000);
    map[1].length = UINT64_C(0x1000);
    map[1].type = 999u;
    assert(nun_boot_info_validate(&info, map, 2u) ==
           NUN_STATUS_INVALID_MEMORY_MAP);
}

static void test_range_and_framebuffer_checks(void)
{
    nun_boot_info_t info = valid_boot_info();
    info.kernel_image_size = 0u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_INVALID_RANGE);

    info = valid_boot_info();
    info.initrd_size = UINT64_C(0x1000);
    info.initrd_physical_base = UINT64_MAX - UINT64_C(0x0fff);
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_INVALID_RANGE);

    info = valid_boot_info();
    info.framebuffer_physical_address = UINT64_C(0x00A00000);
    info.framebuffer_width = 800u;
    assert(nun_boot_info_validate(&info, valid_map, 2u) ==
           NUN_STATUS_INVALID_FRAMEBUFFER);
}

static void test_status_names(void)
{
    assert(strcmp(nun_status_name(NUN_STATUS_OK), "ok") == 0);
    assert(strcmp(nun_status_name(NUN_STATUS_BAD_MAGIC), "bad-magic") == 0);
    assert(strcmp(nun_status_name((nun_status_t)12345), "unknown-status") == 0);
}

int main(void)
{
    test_valid_boot_info();
    test_null_arguments();
    test_magic_and_abi();
    test_architecture_and_flags();
    test_memory_map_shape();
    test_invalid_and_overlapping_regions();
    test_range_and_framebuffer_checks();
    test_status_names();
    puts("NUN boot-info tests: PASS");
    return 0;
}
