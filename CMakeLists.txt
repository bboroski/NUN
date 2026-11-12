cmake_minimum_required(VERSION 3.20)
project(NUN VERSION 0.1.0 LANGUAGES C)

option(NUN_BUILD_TESTS "Build NUN and imported module tests" ON)
set(REAPER_BUILD_TESTS ${NUN_BUILD_TESTS} CACHE BOOL "ReaperCripple host tests" FORCE)
if(NOT NUN_BUILD_TESTS)
    set(BUILD_TESTING OFF CACHE BOOL "Disable imported module test executables" FORCE)
endif()
add_subdirectory(modules/ReaperCripple)
add_subdirectory(modules/ToolNG)

add_library(NUN.Kloz STATIC
    modules/Kloz/src/arena.c
    modules/Kloz/src/budget.c
    modules/Kloz/src/platform.c
)
target_include_directories(NUN.Kloz PUBLIC modules/Kloz/include)

add_library(NUN.CO STATIC modules/CO/src/runtime.c)
target_include_directories(NUN.CO PUBLIC modules/CO/include)

add_library(NUN.SaintSI STATIC
    modules/SaintSI/src/crc32.c
    modules/SaintSI/src/health.c
)
target_include_directories(NUN.SaintSI PUBLIC modules/SaintSI/include)

add_library(NUN.Runtime STATIC src/runtime/runtime.c src/runtime/advice.c)
target_include_directories(NUN.Runtime
    PUBLIC
        include
        modules/Kloz/include
        modules/CO/include
        modules/SaintSI/include
        modules/ReaperCripple/include
    PRIVATE modules/ToolNG/src
)
target_link_libraries(NUN.Runtime
    PUBLIC
        NUN.Kloz
        NUN.CO
        NUN.SaintSI
        efmf
        ToolNG.PolicyCore
)
if(MSVC)
    target_compile_options(NUN.Runtime PRIVATE /W4 /utf-8)
else()
    target_compile_options(NUN.Runtime PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_library(NUN.ServiceGraph STATIC src/services/graph.c)
target_include_directories(NUN.ServiceGraph PUBLIC include)
target_compile_features(NUN.ServiceGraph PUBLIC c_std_11)
if(MSVC)
    target_compile_options(NUN.ServiceGraph PRIVATE /W4)
else()
    target_compile_options(NUN.ServiceGraph PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_library(NUN.BootContract STATIC src/boot_info.c)
target_include_directories(NUN.BootContract PUBLIC include)
target_compile_features(NUN.BootContract PUBLIC c_std_11)
if(MSVC)
    target_compile_options(NUN.BootContract PRIVATE /W4)
else()
    target_compile_options(NUN.BootContract PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(NUN_BUILD_TESTS)
    enable_testing()

    add_executable(NUN.ServiceGraphTests tests/test_service_graph.c)
    target_link_libraries(NUN.ServiceGraphTests PRIVATE NUN.ServiceGraph)
    if(NOT MSVC)
        target_compile_options(NUN.ServiceGraphTests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    add_test(NAME nun_service_graph_tests COMMAND NUN.ServiceGraphTests)

    add_executable(NUN.BootInfoTests tests/test_boot_info.c)
    target_link_libraries(NUN.BootInfoTests PRIVATE NUN.BootContract)
    if(NOT MSVC)
        target_compile_options(NUN.BootInfoTests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    add_test(NAME nun_boot_info_contract COMMAND NUN.BootInfoTests)

    add_executable(NUN.RuntimeTests tests/runtime_tests.c)
    target_link_libraries(NUN.RuntimeTests PRIVATE NUN.Runtime)
    add_test(NAME nun_runtime_tests COMMAND NUN.RuntimeTests)

    add_executable(NUN.AdviceTests tests/advice_tests.c)
    target_link_libraries(NUN.AdviceTests PRIVATE NUN.Runtime)
    add_test(NAME nun_runtime_advice_tests COMMAND NUN.AdviceTests)

    add_executable(NUN.KlozArenaTests modules/Kloz/tests/test_arena.c)
    target_link_libraries(NUN.KlozArenaTests PRIVATE NUN.Kloz)
    add_test(NAME nun_kloz_arena_tests COMMAND NUN.KlozArenaTests)

    add_executable(NUN.KlozPlatformTests modules/Kloz/tests/test_platform.c)
    target_link_libraries(NUN.KlozPlatformTests PRIVATE NUN.Kloz)
    add_test(NAME nun_kloz_platform_tests COMMAND NUN.KlozPlatformTests)

    add_executable(NUN.KlozCompatTests modules/Kloz/tests/test_compat.c)
    target_link_libraries(NUN.KlozCompatTests PRIVATE NUN.Kloz)
    add_test(NAME nun_kloz_compat_tests COMMAND NUN.KlozCompatTests)

    add_executable(NUN.COTests modules/CO/tests/test_runtime.c)
    target_link_libraries(NUN.COTests PRIVATE NUN.CO)
    add_test(NAME nun_co_tests COMMAND NUN.COTests)

    add_executable(NUN.COCompatTests modules/CO/tests/test_compat.c)
    target_link_libraries(NUN.COCompatTests PRIVATE NUN.CO)
    add_test(NAME nun_co_compat_tests COMMAND NUN.COCompatTests)

    add_executable(NUN.SaintSITests modules/SaintSI/tests/test_saintsi.c)
    target_link_libraries(NUN.SaintSITests PRIVATE NUN.SaintSI)
    add_test(NAME nun_saintsi_tests COMMAND NUN.SaintSITests)

    add_executable(NUN.SaintSICompatTests modules/SaintSI/tests/test_compat.c)
    target_link_libraries(NUN.SaintSICompatTests PRIVATE NUN.SaintSI)
    add_test(NAME nun_saintsi_compat_tests COMMAND NUN.SaintSICompatTests)
endif()

message(STATUS "NUN integrates seven pinned source repositories. This host build is not a bootable OS image.")
