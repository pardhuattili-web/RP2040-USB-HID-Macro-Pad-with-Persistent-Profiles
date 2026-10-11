cmake_minimum_required(VERSION 3.13)

option(BUILD_HOST_TESTS "Build portable unit tests without the Pico SDK" OFF)

if(BUILD_HOST_TESTS)
    project(rp2040_macro_pad_host_tests C)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
    enable_testing()

    add_executable(test_core
        tests/test_core.c
        src/button.c
        src/config_store.c
        src/crc32.c
        src/macro_engine.c
    )
    target_include_directories(test_core PRIVATE include)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME core_tests COMMAND test_core)
    return()
endif()

include(pico_sdk_import.cmake)
project(rp2040_usb_hid_macro_pad C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()

add_executable(macro_pad
    src/main.c
    src/button.c
    src/config_store.c
    src/crc32.c
    src/macro_engine.c
)

target_include_directories(macro_pad PRIVATE include)
target_link_libraries(macro_pad
    pico_stdlib
    hardware_gpio
    tinyusb_device
    tinyusb_board
)

pico_enable_stdio_usb(macro_pad 0)
pico_enable_stdio_uart(macro_pad 0)
pico_add_extra_outputs(macro_pad)
