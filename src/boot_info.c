#include "nun/boot_info.h"

#include <stddef.h>
#include <stdint.h>

static int nun_u64_add_overflows(uint64_t left, uint64_t right)
{
    return right > (UINT64_MAX - left);
}

static int nun_architecture_valid(uint32_t architecture)
{
    return architecture >= (uint32_t)NUN_ARCH_X86_64 &&
           architecture <= (uint32_t)NUN_ARCH_E2K;
}

static int nun_memory_type_valid(uint32_t type)
{
    return type >= (uint32_t)NUN_MEMORY_USABLE &&
           type <= (uint32_t)NUN_MEMORY_INITRD;
}

static nun_boot_info_status_t nun_validate_memory_map(
    const nun_boot_info_v1_t *info,
    const nun_memory_region_v1_t *regions,
    uint32_t region_count)
{
    uint32_t index;
    uint64_t previous_end = 0;

    if (region_count == 0 ||
        region_count > NUN_BOOT_INFO_MAX_MEMORY_REGIONS ||
        region_count != info->memory_region_count ||
        regions == NULL ||
        info->memory_regions_physical_address == 0) {
        return NUN_BOOT_INFO_BAD_MEMORY_MAP;
    }

    for (index = 0; index < region_count; ++index) {
        const nun_memory_region_v1_t *region = &regions[index];
        uint64_t end;

        if (region->length == 0 ||
            nun_u64_add_overflows(region->base, region->length) ||
            !nun_memory_type_valid(region->type)) {
            return NUN_BOOT_INFO_BAD_REGION;
        }

        end = region->base + region->length;
        if (index > 0 && region->base < previous_end) {
            return NUN_BOOT_INFO_BAD_REGION;
        }
        previous_end = end;
    }

    return NUN_BOOT_INFO_OK;
}

static nun_boot_info_status_t nun_validate_kernel(const nun_boot_info_v1_t *info)
{
    uint64_t kernel_end;

    if (info->kernel_physical_base == 0 ||
        info->kernel_size == 0 ||
        nun_u64_add_overflows(info->kernel_physical_base, info->kernel_size)) {
        return NUN_BOOT_INFO_BAD_KERNEL_RANGE;
    }

    kernel_end = info->kernel_physical_base + info->kernel_size;
    if (info->kernel_entry_physical < info->kernel_physical_base ||
        info->kernel_entry_physical >= kernel_end) {
        return NUN_BOOT_INFO_BAD_KERNEL_RANGE;
    }

    return NUN_BOOT_INFO_OK;
}

static nun_boot_info_status_t nun_validate_initrd(const nun_boot_info_v1_t *info)
{
    if (info->initrd_size == 0) {
        return info->initrd_physical_base == 0
                   ? NUN_BOOT_INFO_OK
                   : NUN_BOOT_INFO_BAD_INITRD;
    }

    if (info->initrd_physical_base == 0 ||
        nun_u64_add_overflows(info->initrd_physical_base, info->initrd_size)) {
        return NUN_BOOT_INFO_BAD_INITRD;
    }

    return NUN_BOOT_INFO_OK;
}

static nun_boot_info_status_t nun_validate_framebuffer(const nun_framebuffer_v1_t *fb)
{
    uint64_t bytes_per_pixel;
    uint64_t row_bytes;
    uint64_t required_bytes;

    if (fb->byte_size == 0) {
        if (fb->physical_base != 0 || fb->width != 0 || fb->height != 0 ||
            fb->pixels_per_scanline != 0 || fb->pixel_format != NUN_PIXEL_NONE) {
            return NUN_BOOT_INFO_BAD_FRAMEBUFFER;
        }
        return NUN_BOOT_INFO_OK;
    }

    if (fb->physical_base == 0 || fb->width == 0 || fb->height == 0 ||
        fb->pixels_per_scanline < fb->width ||
        nun_u64_add_overflows(fb->physical_base, fb->byte_size)) {
        return NUN_BOOT_INFO_BAD_FRAMEBUFFER;
    }

    switch (fb->pixel_format) {
    case NUN_PIXEL_RGB565:
        bytes_per_pixel = 2;
        break;
    case NUN_PIXEL_XRGB8888:
    case NUN_PIXEL_BGRX8888:
        bytes_per_pixel = 4;
        break;
    default:
        return NUN_BOOT_INFO_BAD_FRAMEBUFFER;
    }

    if ((uint64_t)fb->pixels_per_scanline > UINT64_MAX / bytes_per_pixel) {
        return NUN_BOOT_INFO_BAD_FRAMEBUFFER;
    }
    row_bytes = (uint64_t)fb->pixels_per_scanline * bytes_per_pixel;

    if ((uint64_t)fb->height > UINT64_MAX / row_bytes) {
        return NUN_BOOT_INFO_BAD_FRAMEBUFFER;
    }
    required_bytes = row_bytes * (uint64_t)fb->height;

    if (required_bytes > fb->byte_size) {
        return NUN_BOOT_INFO_BAD_FRAMEBUFFER;
    }

    return NUN_BOOT_INFO_OK;
}

nun_boot_info_status_t nun_boot_info_validate_v1(
    const nun_boot_info_v1_t *info,
    const nun_memory_region_v1_t *regions,
    uint32_t region_count)
{
    nun_boot_info_status_t status;

    if (info == NULL) {
        return NUN_BOOT_INFO_INVALID_ARGUMENT;
    }
    if (info->magic != NUN_BOOT_INFO_MAGIC) {
        return NUN_BOOT_INFO_BAD_MAGIC;
    }
    if (info->struct_size < (uint32_t)sizeof(*info)) {
        return NUN_BOOT_INFO_BAD_SIZE;
    }
    if (info->abi_major != NUN_BOOT_INFO_ABI_MAJOR) {
        return NUN_BOOT_INFO_UNSUPPORTED_ABI;
    }
    if (!nun_architecture_valid(info->architecture)) {
        return NUN_BOOT_INFO_BAD_ARCHITECTURE;
    }

    status = nun_validate_memory_map(info, regions, region_count);
    if (status != NUN_BOOT_INFO_OK) {
        return status;
    }

    status = nun_validate_kernel(info);
    if (status != NUN_BOOT_INFO_OK) {
        return status;
    }

    status = nun_validate_initrd(info);
    if (status != NUN_BOOT_INFO_OK) {
        return status;
    }

    return nun_validate_framebuffer(&info->framebuffer);
}

const char *nun_boot_info_status_string(nun_boot_info_status_t status)
{
    switch (status) {
    case NUN_BOOT_INFO_OK:
        return "ok";
    case NUN_BOOT_INFO_INVALID_ARGUMENT:
        return "invalid argument";
    case NUN_BOOT_INFO_BAD_MAGIC:
        return "invalid boot information magic";
    case NUN_BOOT_INFO_BAD_SIZE:
        return "boot information structure is too small";
    case NUN_BOOT_INFO_UNSUPPORTED_ABI:
        return "unsupported boot information ABI";
    case NUN_BOOT_INFO_BAD_ARCHITECTURE:
        return "unsupported architecture identifier";
    case NUN_BOOT_INFO_BAD_MEMORY_MAP:
        return "invalid memory map";
    case NUN_BOOT_INFO_BAD_REGION:
        return "invalid or overlapping memory region";
    case NUN_BOOT_INFO_BAD_KERNEL_RANGE:
        return "invalid kernel physical range or entry point";
    case NUN_BOOT_INFO_BAD_INITRD:
        return "invalid initrd range";
    case NUN_BOOT_INFO_BAD_FRAMEBUFFER:
        return "invalid framebuffer description";
    default:
        return "unknown boot information status";
    }
}
