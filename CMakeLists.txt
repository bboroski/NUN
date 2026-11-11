cmake_minimum_required(VERSION 3.20)
project(NUN VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

add_library(nun_boot_contract STATIC
    src/boot_info.c
)
target_include_directories(nun_boot_contract PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(nun_boot_contract PRIVATE /W4)
else()
    target_compile_options(nun_boot_contract PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(BUILD_TESTING)
    add_executable(nun_boot_info_tests tests/test_boot_info.c)
    target_link_libraries(nun_boot_info_tests PRIVATE nun_boot_contract)
    if(NOT MSVC)
        target_compile_options(nun_boot_info_tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    add_test(NAME nun_boot_info_contract COMMAND nun_boot_info_tests)
endif()
