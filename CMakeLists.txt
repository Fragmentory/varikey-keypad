# SPDX-FileCopyrightText: 2023 Roman Koch <koch.roman@gmail.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.22)

set(CMAKE_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}/.cmake")
set(SCRIPTS_DIR "${CMAKE_CURRENT_LIST_DIR}/.scripts")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Firmware build type")
endif()

option(PICO_SDK_FETCH_FROM_GIT "Fetch the Pico SDK when PICO_SDK_PATH is unset" ON)
# Raspberry Pi Pico SDK 2.3.1
set(PICO_SDK_FETCH_FROM_GIT_TAG "079c6f39023649b154152db30f1d781e884879bc" CACHE STRING "Pico SDK revision")
include("${CMAKE_MODULE_DIR}/pico_sdk_import.cmake")
include("${CMAKE_MODULE_DIR}/pico_extras_import_optional.cmake")
include("${CMAKE_MODULE_DIR}/cpp_check.cmake")

project(varikey
    VERSION 1.1.0.2
    DESCRIPTION "Custom composed keypad HID device"
    LANGUAGES C CXX ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
pico_sdk_init()

add_compile_options(
    -Wall
    -Wno-format # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    #-Wno-unused-parameter # we have some for the docs that aren't called
    #-Wno-unused-variable # we have some for the docs that aren't called
    #-Wno-unused-but-set-variable # we have some for the docs that aren't called
    #-Wno-maybe-uninitialized
    #-Wextra
    -Waddress
    #-Wpedantic
    #-fpack-struct
)

set(HARDWARE_PLATFORM "VARIKEY" CACHE STRING "Keypad board")
set_property(CACHE HARDWARE_PLATFORM PROPERTY STRINGS VARIKEY GMCI VARIKEY_15SBLA_PROTO)
if(HARDWARE_PLATFORM STREQUAL "GMCI")
    set(HARDWARE_IDENTIFIER 2)
    set(HARDWARE_NUMBER 1)
    set(HARDWARE_VARIANT 0)
elseif(HARDWARE_PLATFORM STREQUAL "VARIKEY")
    set(HARDWARE_IDENTIFIER 1)
    set(HARDWARE_NUMBER 1)
    set(HARDWARE_VARIANT 0)
elseif(HARDWARE_PLATFORM STREQUAL "VARIKEY_15SBLA_PROTO")
    set(HARDWARE_IDENTIFIER 3)
    set(HARDWARE_NUMBER 1)
    set(HARDWARE_VARIANT 0)
else()
    message(FATAL_ERROR "Unsupported HARDWARE_PLATFORM: ${HARDWARE_PLATFORM}")
endif()
message(STATUS "platform is ${HARDWARE_PLATFORM} (internal ID:${HARDWARE_IDENTIFIER} NUM:${HARDWARE_NUMBER} VAR:${HARDWARE_VARIANT})")

set(FIRMWARE_PRODUCT "KEYPAD_DEMO" CACHE STRING "Keypad firmware product")
set_property(CACHE FIRMWARE_PRODUCT PROPERTY STRINGS KEYPAD_DEMO KEYPAD_10BWDB KEYPAD_15SBLA KEYPAD_PANEL15)
if(FIRMWARE_PRODUCT STREQUAL "KEYPAD_10BWDB")
    set(FIRMWARE_IDENTIFIER 2)
    set(FIRMWARE_VENDOR 0xCAFE)
elseif(FIRMWARE_PRODUCT STREQUAL "KEYPAD_DEMO")
    set(FIRMWARE_IDENTIFIER 1)
    set(FIRMWARE_VENDOR 0xBEEF)
elseif(FIRMWARE_PRODUCT STREQUAL "KEYPAD_PANEL15")
    if(NOT HARDWARE_PLATFORM STREQUAL "VARIKEY_15SBLA_PROTO")
        message(FATAL_ERROR "KEYPAD_PANEL15 requires VARIKEY_15SBLA_PROTO")
    endif()
    set(FIRMWARE_IDENTIFIER 4)
    set(FIRMWARE_VENDOR 0xABCD)
    add_compile_definitions(VARIKEY_PANEL15)
elseif(FIRMWARE_PRODUCT STREQUAL "KEYPAD_15SBLA")
    set(FIRMWARE_IDENTIFIER 3)
    set(FIRMWARE_VENDOR 0xABCD)
else()
    message(FATAL_ERROR "Unsupported FIRMWARE_PRODUCT: ${FIRMWARE_PRODUCT}")
endif()
message(STATUS "product is ${FIRMWARE_PRODUCT} (internal ID:${FIRMWARE_IDENTIFIER} REV:${PROJECT_VERSION})")

set(EXECUTABLE_NAME "${PROJECT_NAME}_${FIRMWARE_PRODUCT}_${HARDWARE_PLATFORM}_${HARDWARE_NUMBER}_${HARDWARE_VARIANT}")
string(TOLOWER ${EXECUTABLE_NAME} EXECUTABLE_NAME)

set(VARIKEY_LIBRARY_NAME ${CMAKE_PROJECT_NAME}_library)
SET(VARIKEY_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${VARIKEY_LIBRARY_NAME}")
string(TOLOWER ${VARIKEY_LIBRARY_NAME} VARIKEY_LIBRARY_NAME)

set(GIT_HASH_SIZE 8)
set(GIT_HASH "00000000")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(COMMAND git rev-parse --short=${GIT_HASH_SIZE} HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
endif()
configure_file("${SCRIPTS_DIR}/revision.h.in" "generated/revision.h" @ONLY)

include("${CMAKE_MODULE_DIR}/graffiti.cmake")
  
add_executable(${EXECUTABLE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/firmware.cpp
)

add_dependencies(${EXECUTABLE_NAME} ascii_art)

add_subdirectory(${VARIKEY_LIBRARY_NAME})

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
# target_compile_definitions(dev_hid_composite PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
    ${VARIKEY_LIBRARY_NAME}
)

# add url via pico_set_program_url
# example_auto_set_url(${PROJECT_NAME})

# pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${EXECUTABLE_NAME} 1)
set(flash_profile "${CMAKE_CURRENT_SOURCE_DIR}/boards/rp2040-2m-flash.json")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${flash_profile}")
file(READ "${flash_profile}" flash_layout)
foreach(field flash_base flash_bytes erase_bytes service_offset settings_a_offset settings_b_offset)
    string(JSON ${field} GET "${flash_layout}" "${field}")
endforeach()
set(flash_limits "${CMAKE_CURRENT_BINARY_DIR}/keypad-flash.ld")
configure_file("${CMAKE_MODULE_DIR}/keypad-flash.ld.in" "${flash_limits}" @ONLY)
target_link_options(${EXECUTABLE_NAME} PRIVATE "LINKER:-T,${flash_limits}")
set_property(TARGET ${EXECUTABLE_NAME} APPEND PROPERTY LINK_DEPENDS "${flash_limits}")
pico_add_extra_outputs(${EXECUTABLE_NAME})

if(NOT PICO_NO_UF2 AND NOT PICO_NO_PICOTOOL)
    set(application_uf2 "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/${EXECUTABLE_NAME}.uf2")
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND "${SCRIPTS_DIR}/check-uf2.py" "${application_uf2}"
        COMMENT "Check all UF2 pages against the keypad application area"
        VERBATIM)
    set_property(TARGET ${EXECUTABLE_NAME} APPEND PROPERTY LINK_DEPENDS "${SCRIPTS_DIR}/check-uf2.py")
    # The SDK's UF2 converter can be built without USB. Provision with a host tool.
    find_program(VARIKEY_HOST_PICOTOOL NAMES picotool NO_CMAKE_FIND_ROOT_PATH)
    set(provision_tool picotool)
    if(VARIKEY_HOST_PICOTOOL)
        set(provision_tool "${VARIKEY_HOST_PICOTOOL}")
    endif()
    add_custom_target(provision
        COMMAND "${CMAKE_COMMAND}" -E env "PICOTOOL=${provision_tool}"
                "${SCRIPTS_DIR}/provision.sh"
                "${application_uf2}"
        DEPENDS ${EXECUTABLE_NAME}
        USES_TERMINAL
        VERBATIM
    )
endif()
