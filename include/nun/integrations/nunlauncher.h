#ifndef NUN_INTEGRATION_NUNLAUNCHER_H
#define NUN_INTEGRATION_NUNLAUNCHER_H

#include <stddef.h>
#include <stdint.h>

#include "nun/boot_info.h"
#include "nunlauncher/boot_contract.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Host-side adapter between NUN's memory-map vocabulary and NUNLauncher's
 * pre-load image admission contract. It does not load, map, authenticate,
 * relocate or execute an image and is not part of the freestanding kernel.
 */
nunl_result_t nun_boot_admission_validate_image(
    const void *image,
    size_t available_bytes,
    uint64_t load_address,
    const nun_memory_region_t *regions,
    size_t region_count,
    nun_architecture_t target_architecture,
    uint64_t *entry_address_out);

#ifdef __cplusplus
}
#endif

#endif
