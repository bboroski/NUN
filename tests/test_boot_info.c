#include "nun/boot_info.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            (void)fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__,     \
                          #condition);                                          \
            return 1;                                                           \
        }                                                                       \
    } while (0)

static void make_valid(
    nun_boot_info_v1_t *info,
    nun_memory_region_v1_t regions[2])
{
    (void)memset(info, 0, sizeof(*info));
    (void)memset(regions, 0, sizeof(nun_memory_region_v1_t) * 2u);

    info->magic = NUN_BOOT_INFO_MAGIC;
    info->struct_size = (uint32_t)sizeof(*info);
    info->abi_major = NUN_BOOT_INFO_ABI_MAJOR;
    info->abi_minor = NUN_BOOT_INFO_ABI_MINOR;
    info->architecture = NUN_ARCH_X86_64;
    info->memory_region_count = 2;
    info->memory_regions_physical_address = UINT64_C(0x100000);
    info->kernel_physical_base = UINT64_C(0x200000);
    info->kernel_size = UINT64_C(0x100000);
    info->kernel_entry_physical = UINT64_C(0x201000);

    regions[0].base = UINT64_C(0x100000);
    regions[0].length = UINT64_C(0x100000);
    regions[0].type = NUN_MEMORY_RESERVED;

    regions[1].base = UINT64_C(0x200000);
    regions[1].length = UINT64_C(0x200000);
    regions[1].type = NUN_MEMORY_KERNEL;
}

int main(void)
{
    nun_boot_info_v1_t info;
    nun_memory_region_v1_t regions[2];

    make_valid(&info, regions);
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) == NUN_BOOT_INFO_OK);

    CHECK(nun_boot_info_validate_v1(NULL, regions, 2) ==
          NUN_BOOT_INFO_INVALID_ARGUMENT);

    make_valid(&info, regions);
    info.magic = 0;
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) ==
          NUN_BOOT_INFO_BAD_MAGIC);

    make_valid(&info, regions);
    info.abi_major = 2;
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) ==
          NUN_BOOT_INFO_UNSUPPORTED_ABI);

    make_valid(&info, regions);
    regions[1].base = regions[0].base + regions[0].length - 1;
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) ==
          NUN_BOOT_INFO_BAD_REGION);

    make_valid(&info, regions);
    regions[0].base = UINT64_MAX - 8;
    regions[0].length = 16;
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) ==
          NUN_BOOT_INFO_BAD_REGION);

    make_valid(&info, regions);
    info.kernel_entry_physical = info.kernel_physical_base + info.kernel_size;
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) ==
          NUN_BOOT_INFO_BAD_KERNEL_RANGE);

    make_valid(&info, regions);
    info.initrd_physical_base = UINT64_C(0x500000);
    info.initrd_size = UINT64_C(0x2000);
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) == NUN_BOOT_INFO_OK);

    make_valid(&info, regions);
    info.framebuffer.physical_base = UINT64_C(0x800000);
    info.framebuffer.byte_size = UINT64_C(640) * 480u * 4u;
    info.framebuffer.width = 640;
    info.framebuffer.height = 480;
    info.framebuffer.pixels_per_scanline = 640;
    info.framebuffer.pixel_format = NUN_PIXEL_XRGB8888;
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) == NUN_BOOT_INFO_OK);

    make_valid(&info, regions);
    info.framebuffer.physical_base = UINT64_C(0x800000);
    info.framebuffer.byte_size = 16;
    info.framebuffer.width = 640;
    info.framebuffer.height = 480;
    info.framebuffer.pixels_per_scanline = 640;
    info.framebuffer.pixel_format = NUN_PIXEL_XRGB8888;
    CHECK(nun_boot_info_validate_v1(&info, regions, 2) ==
          NUN_BOOT_INFO_BAD_FRAMEBUFFER);

    make_valid(&info, regions);
    CHECK(nun_boot_info_validate_v1(&info, regions, 1) ==
          NUN_BOOT_INFO_BAD_MEMORY_MAP);

    (void)puts("NUN boot-information tests: PASS");
    return 0;
}
