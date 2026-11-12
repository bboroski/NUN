#include "nun/integrations/nunlauncher.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void put_le32(uint8_t *bytes, uint32_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

static void put_le64(uint8_t *bytes, uint64_t value)
{
    put_le32(bytes, (uint32_t)value);
    put_le32(bytes + 4u, (uint32_t)(value >> 32));
}

static void make_image(uint8_t image[64])
{
    memset(image, 0, 64u);
    memcpy(image, NUNL_IMAGE_MAGIC, NUNL_IMAGE_MAGIC_SIZE);
    put_le32(image + 8u, 1u);
    put_le32(image + 12u, NUNL_IMAGE_HEADER_SIZE);
    put_le64(image + 16u, 64u);
    put_le64(image + 24u, NUNL_IMAGE_HEADER_SIZE);
    put_le32(image + 32u, (uint32_t)NUNL_ARCH_X86_64);
    put_le32(image + 40u, nunl_image_header_crc32(image, 64u));
}

int main(void)
{
    uint8_t image[64];
    uint64_t entry = 0u;
    nun_memory_region_t available = {
        UINT64_C(0x00100000), UINT64_C(0x00002000),
        (uint32_t)NUN_MEMORY_AVAILABLE, 0u
    };
    nun_memory_region_t reserved = {
        UINT64_C(0x00100000), UINT64_C(0x00002000),
        (uint32_t)NUN_MEMORY_KERNEL, 0u
    };

    make_image(image);
    assert(nun_boot_admission_validate_image(
        image, sizeof(image), UINT64_C(0x00100000),
        &available, 1u, NUN_ARCH_X86_64, &entry) == NUNL_OK);
    assert(entry == UINT64_C(0x00100000) + NUNL_IMAGE_HEADER_SIZE);

    assert(nun_boot_admission_validate_image(
        image, sizeof(image), UINT64_C(0x00100000),
        &available, 1u, NUN_ARCH_AARCH64, &entry) == NUNL_EARCH);

    assert(nun_boot_admission_validate_image(
        image, sizeof(image), UINT64_C(0x00100000),
        &reserved, 1u, NUN_ARCH_X86_64, &entry) == NUNL_EMEMORY);

    available.flags = 1u;
    assert(nun_boot_admission_validate_image(
        image, sizeof(image), UINT64_C(0x00100000),
        &available, 1u, NUN_ARCH_X86_64, &entry) == NUNL_EMEMORY);

    assert(nun_boot_admission_validate_image(
        NULL, sizeof(image), UINT64_C(0x00100000),
        &available, 1u, NUN_ARCH_X86_64, &entry) == NUNL_EINVAL);
    return 0;
}
