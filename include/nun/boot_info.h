#ifndef NUN_BOOT_INFO_H
#define NUN_BOOT_INFO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUN_BOOT_INFO_MAGIC UINT32_C(0x4E554E42) /* "NUNB" */
#define NUN_BOOT_INFO_VERSION UINT16_C(1)
#define NUN_BOOT_INFO_MAX_REGIONS 32U

typedef enum {
    NUN_ARCH_X86_64 = 1,
    NUN_ARCH_AARCH64 = 2,
    NUN_ARCH_RISCV64 = 3,
    NUN_ARCH_E2K = 4
} nun_architecture_t;

typedef enum {
    NUN_MEMORY_AVAILABLE = 1,
    NUN_MEMORY_RESERVED = 2,
    NUN_MEMORY_KERNEL = 3,
    NUN_MEMORY_MMIO = 4
} nun_memory_type_t;

typedef enum {
    NUN_BOOT_OK = 0,
    NUN_BOOT_E_ARGUMENT = -1,
    NUN_BOOT_E_MAGIC = -2,
    NUN_BOOT_E_VERSION = -3,
    NUN_BOOT_E_SIZE = -4,
    NUN_BOOT_E_ARCHITECTURE = -5,
    NUN_BOOT_E_RANGE = -6,
    NUN_BOOT_E_MEMORY_MAP = -7,
    NUN_BOOT_E_KERNEL_REGION = -8,
    NUN_BOOT_E_FLAGS = -9
} nun_boot_result_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t flags;
} nun_memory_region_t;

/*
 * Addresses are physical addresses represented as uint64_t.
 * The validator never dereferences memory_map_address; callers pass the
 * separately mapped region array so validation is safe in host tests too.
 */
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t header_size;
    uint32_t total_size;
    uint32_t architecture;
    uint64_t kernel_load_address;
    uint64_t kernel_image_size;
    uint64_t kernel_entry_address;
    uint64_t memory_map_address;
    uint32_t memory_map_count;
    uint32_t flags;
} nun_boot_info_t;

/*
 * Validates a version-1 handoff descriptor and its already-mapped memory map.
 * This function performs no hardware access, allocation, relocation, or jump.
 */
nun_boot_result_t nun_boot_info_validate(
    const nun_boot_info_t *info,
    const nun_memory_region_t *regions,
    size_t region_count);

#ifdef __cplusplus
}
#endif

#endif
