cmake_minimum_required(VERSION 3.16)
project(NUN VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(nun_boot_contract STATIC src/boot_info.c)
target_include_directories(nun_boot_contract PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(CTest)
if(BUILD_TESTING)
    add_executable(nun_boot_contract_tests tests/test_boot_info.c)
    target_link_libraries(nun_boot_contract_tests PRIVATE nun_boot_contract)
    add_test(NAME nun_boot_contract_tests COMMAND nun_boot_contract_tests)
endif()
