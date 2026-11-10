#ifndef NUN_BOOT_INFO_H
#define NUN_BOOT_INFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUN_BOOT_INFO_MAGIC UINT32_C(0x4E554E42)
#define NUN_BOOT_INFO_VERSION 1U
#define NUN_BOOT_MAX_MEMORY_REGIONS 64U

typedef enum {
    NUN_ARCH_UNKNOWN = 0,
    NUN_ARCH_X86_64 = 1,
    NUN_ARCH_AARCH64 = 2,
    NUN_ARCH_RISCV64 = 3,
    /* Contract identifier only; this does not claim a qualified E2K port. */
    NUN_ARCH_E2K = 4
} nun_architecture_t;

typedef enum {
    NUN_MEMORY_AVAILABLE = 1,
    NUN_MEMORY_LOADED_KERNEL = 2,
    NUN_MEMORY_RESERVED = 3,
    NUN_MEMORY_FRAMEBUFFER = 4
} nun_memory_region_type_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} nun_memory_region_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t struct_size;
    uint32_t architecture;
    uint64_t kernel_base;
    uint64_t kernel_size;
    uint64_t entry_address;
    const nun_memory_region_t *memory_regions;
    uint32_t memory_region_count;
    uint32_t flags;
} nun_boot_info_t;

typedef enum {
    NUN_BOOT_OK = 0,
    NUN_BOOT_EINVAL = -1,
    NUN_BOOT_EVERSION = -2,
    NUN_BOOT_EARCH = -3,
    NUN_BOOT_EMEMMAP = -4,
    NUN_BOOT_ERANGE = -5,
    NUN_BOOT_EOVERLAP = -6,
    NUN_BOOT_EENTRY = -7
} nun_boot_status_t;

/*
 * Validate the untrusted handoff block before kernel initialization.
 * This validates metadata only; it does not establish that an architecture
 * port, CPU entry path, or physical device is supported.
 */
nun_boot_status_t nun_boot_info_validate(const nun_boot_info_t *info);

#ifdef __cplusplus
}
#endif

#endif
