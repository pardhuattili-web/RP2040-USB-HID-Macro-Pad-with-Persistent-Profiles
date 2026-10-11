# Import the Raspberry Pi Pico SDK.
# PICO_SDK_PATH may be set in the environment or via CMake cache.
if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

if (NOT PICO_SDK_PATH)
    message(FATAL_ERROR "PICO_SDK_PATH is not set. Set it to your pico-sdk checkout.")
endif()

get_filename_component(PICO_SDK_PATH "${PICO_SDK_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS "${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
    message(FATAL_ERROR "Pico SDK not found at '${PICO_SDK_PATH}'. Check PICO_SDK_PATH.")
endif()

include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
