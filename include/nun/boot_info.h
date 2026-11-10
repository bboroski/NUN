#ifndef NUN_BOOT_INFO_H
#define NUN_BOOT_INFO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUN_BOOT_INFO_MAGIC UINT64_C(0x4E554E424F4F5431)
#define NUN_BOOT_INFO_ABI_MAJOR 1u
#define NUN_BOOT_INFO_ABI_MINOR 0u
#define NUN_BOOT_INFO_MAX_MEMORY_MAP_ENTRIES 256u
#define NUN_BOOT_PAGE_SIZE UINT64_C(4096)

typedef enum {
    NUN_ARCH_UNKNOWN = 0,
    NUN_ARCH_X86_64 = 1,
    NUN_ARCH_AARCH64 = 2,
    NUN_ARCH_RISCV64 = 3
} nun_architecture_t;

typedef enum {
    NUN_MEMORY_USABLE = 1,
    NUN_MEMORY_RESERVED = 2,
    NUN_MEMORY_ACPI_RECLAIMABLE = 3,
    NUN_MEMORY_ACPI_NVS = 4,
    NUN_MEMORY_MMIO = 5,
    NUN_MEMORY_LOADER_RECLAIMABLE = 6,
    NUN_MEMORY_KERNEL = 7,
    NUN_MEMORY_INITRD = 8,
    NUN_MEMORY_FRAMEBUFFER = 9,
    NUN_MEMORY_FIRMWARE = 10
} nun_memory_type_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} nun_memory_region_t;

/*
 * Physical addresses are intentionally represented as uint64_t so the v1
 * structure layout is identical across the supported 64-bit target ABIs.
 * The kernel must map the memory-map storage before validating its entries.
 */
typedef struct {
    uint64_t magic;
    uint32_t struct_size;
    uint16_t abi_major;
    uint16_t abi_minor;
    uint32_t architecture;
    uint32_t flags;

    uint64_t memory_map_physical_address;
    uint32_t memory_map_count;
    uint32_t memory_map_entry_size;

    uint64_t kernel_physical_base;
    uint64_t kernel_image_size;
    uint64_t initrd_physical_base;
    uint64_t initrd_size;

    uint64_t acpi_rsdp_physical_address;
    uint64_t device_tree_physical_address;

    uint64_t framebuffer_physical_address;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_format;
} nun_boot_info_t;

typedef enum {
    NUN_STATUS_OK = 0,
    NUN_STATUS_INVALID_ARGUMENT = -1,
    NUN_STATUS_BAD_MAGIC = -2,
    NUN_STATUS_UNSUPPORTED_ABI = -3,
    NUN_STATUS_UNSUPPORTED_ARCHITECTURE = -4,
    NUN_STATUS_INVALID_MEMORY_MAP = -5,
    NUN_STATUS_MEMORY_MAP_OVERLAP = -6,
    NUN_STATUS_INVALID_RANGE = -7,
    NUN_STATUS_INVALID_FRAMEBUFFER = -8,
    NUN_STATUS_RESERVED_FLAGS = -9
} nun_status_t;

/*
 * mapped_memory_map must already refer to readable memory owned/mapped by the
 * caller. This function validates the supplied entries; it cannot safely
 * dereference an arbitrary physical address from boot_info.
 *
 * mapped_entry_count must equal boot_info->memory_map_count.
 */
nun_status_t nun_boot_info_validate(
    const nun_boot_info_t *boot_info,
    const nun_memory_region_t *mapped_memory_map,
    size_t mapped_entry_count);

const char *nun_status_name(nun_status_t status);

#ifdef __cplusplus
}
#endif

#endif
