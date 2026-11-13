#if !defined(__e2k__) || !defined(__linux__)
#error "This interop qualification test must be compiled for native E2K Linux."
#endif

#include "co/platform.h"
#include "kloz/arch.h"
#include "reaper_bsp_e2k.h"
#include "reaper_core.h"
#include "toolng_platform.h"

#include <stdio.h>

#define REQUIRE(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "E2K interop qualification failed at %s:%d: %s", __FILE__, __LINE__, #condition); \
                      __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    const kloz_arch_info_t kloz = kloz_arch_detect();
    const co_platform_info_t co = co_platform_detect();
    const ToolNGPlatformInfo toolng = toolng_platform_info();

    REQUIRE(sizeof(void *) == 8u);

    REQUIRE(kloz.architecture == KLOZ_ARCH_E2K);
    REQUIRE(kloz.pointer_bits == 64u);
    REQUIRE(kloz.little_endian == 1u);
    REQUIRE(kloz.elf_machine == 175u);

    REQUIRE(co.architecture == CO_ARCH_E2K);
    REQUIRE(co.pointer_bits == 64u);
    REQUIRE(co.little_endian == 1u);
    REQUIRE(co.elf_machine == 175u);

    REQUIRE(toolng.architecture == TOOLNG_ARCH_E2K);
    REQUIRE(toolng.pointer_bits == 64u);
    REQUIRE(toolng.little_endian == 1u);
    REQUIRE(toolng.elf_machine == 175u);

    REQUIRE(reaper_compile_target_architecture_is_e2k() == 1);
    REQUIRE(reaper_compile_target_architecture() == ARCH_TARGET_E2K);

    (void)puts("E2K interop qualification passed: Kloz, CO, ToolNG and ReaperCripple agree.");
    return 0;
}
