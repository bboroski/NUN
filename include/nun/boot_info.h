#ifndef NUN_BOOT_INFO_H
#define NUN_BOOT_INFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUN_BOOT_INFO_MAGIC UINT64_C(0x4E554E424F4F5431)
#define NUN_BOOT_INFO_ABI_MAJOR 1u
#define NUN_BOOT_INFO_ABI_MINOR 0u
#define NUN_BOOT_INFO_MAX_MEMORY_REGIONS 128u

typedef enum nun_architecture {
    NUN_ARCH_UNKNOWN = 0,
    NUN_ARCH_X86_64 = 1,
    NUN_ARCH_AARCH64 = 2,
    NUN_ARCH_RISCV64 = 3,
    NUN_ARCH_E2K = 4
} nun_architecture_t;

typedef enum nun_memory_type {
    NUN_MEMORY_USABLE = 1,
    NUN_MEMORY_RESERVED = 2,
    NUN_MEMORY_ACPI_RECLAIMABLE = 3,
    NUN_MEMORY_ACPI_NVS = 4,
    NUN_MEMORY_MMIO = 5,
    NUN_MEMORY_FRAMEBUFFER = 6,
    NUN_MEMORY_BOOTLOADER_RECLAIMABLE = 7,
    NUN_MEMORY_KERNEL = 8,
    NUN_MEMORY_INITRD = 9
} nun_memory_type_t;

typedef enum nun_pixel_format {
    NUN_PIXEL_NONE = 0,
    NUN_PIXEL_RGB565 = 1,
    NUN_PIXEL_XRGB8888 = 2,
    NUN_PIXEL_BGRX8888 = 3
} nun_pixel_format_t;

enum {
    NUN_BOOT_FLAG_UEFI = UINT64_C(1) << 0,
    NUN_BOOT_FLAG_FIRMWARE_SECURE_BOOT_ENABLED = UINT64_C(1) << 1,
    NUN_BOOT_FLAG_ACPI_PRESENT = UINT64_C(1) << 2,
    NUN_BOOT_FLAG_DEVICE_TREE_PRESENT = UINT64_C(1) << 3,
    NUN_BOOT_FLAG_FRAMEBUFFER_PRESENT = UINT64_C(1) << 4,
    NUN_BOOT_FLAG_INITRD_PRESENT = UINT64_C(1) << 5
};

typedef struct nun_memory_region_v1 {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} nun_memory_region_v1_t;

typedef struct nun_framebuffer_v1 {
    uint64_t physical_base;
    uint64_t byte_size;
    uint32_t width;
    uint32_t height;
    uint32_t pixels_per_scanline;
    uint32_t pixel_format;
} nun_framebuffer_v1_t;

/*
 * All addresses in this structure are physical addresses.
 * The structure contains no native pointers and is not serialized with
 * compiler-dependent pointer or bit-field layouts.
 */
typedef struct nun_boot_info_v1 {
    uint64_t magic;
    uint32_t struct_size;
    uint16_t abi_major;
    uint16_t abi_minor;
    uint32_t architecture;
    uint32_t memory_region_count;
    uint64_t flags;
    uint64_t memory_regions_physical_address;
    uint64_t acpi_rsdp_physical_address;
    uint64_t device_tree_physical_address;
    uint64_t kernel_physical_base;
    uint64_t kernel_size;
    uint64_t kernel_entry_physical;
    uint64_t initrd_physical_base;
    uint64_t initrd_size;
    nun_framebuffer_v1_t framebuffer;
    uint8_t kernel_sha256[32];
} nun_boot_info_v1_t;

typedef enum nun_boot_info_status {
    NUN_BOOT_INFO_OK = 0,
    NUN_BOOT_INFO_INVALID_ARGUMENT = 1,
    NUN_BOOT_INFO_BAD_MAGIC = 2,
    NUN_BOOT_INFO_BAD_SIZE = 3,
    NUN_BOOT_INFO_UNSUPPORTED_ABI = 4,
    NUN_BOOT_INFO_BAD_ARCHITECTURE = 5,
    NUN_BOOT_INFO_BAD_MEMORY_MAP = 6,
    NUN_BOOT_INFO_BAD_REGION = 7,
    NUN_BOOT_INFO_BAD_KERNEL_RANGE = 8,
    NUN_BOOT_INFO_BAD_INITRD = 9,
    NUN_BOOT_INFO_BAD_FRAMEBUFFER = 10
} nun_boot_info_status_t;

/*
 * Validate scalar metadata and the supplied copy of the memory-map entries.
 * This function does not dereference physical addresses, map memory, verify
 * signatures, or prove that the loader actually exited firmware services.
 */
nun_boot_info_status_t nun_boot_info_validate_v1(
    const nun_boot_info_v1_t *info,
    const nun_memory_region_v1_t *regions,
    uint32_t region_count);

const char *nun_boot_info_status_string(nun_boot_info_status_t status);

#ifdef __cplusplus
}
#endif

#endif
