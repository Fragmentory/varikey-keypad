# SPDX-FileCopyrightText: 2026 Roman Koch
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.22)
project(varikey_firmware_tests LANGUAGES CXX)
enable_testing()
set(lib "${CMAKE_CURRENT_LIST_DIR}/../varikey_library")
set(interface "${lib}/engine/interface")
add_executable(reset_command_test reset_command_test.cpp
    "${lib}/engine/reset_control.cpp"
    "${interface}/payload/payload_reset.cpp"
    "${interface}/hci/commander/control/cmd_reset_msg.cpp")
target_compile_features(reset_command_test PRIVATE cxx_std_20)
target_include_directories(reset_command_test PRIVATE "${lib}" "${lib}/pulp" "${lib}/registry"
    "${lib}/engine" "${interface}/payload" "${interface}/hci/commander"
    "${interface}/hci/commander/control" "${interface}/hci/serial")
add_test(NAME reset_command COMMAND reset_command_test)

set(scanner "${lib}/platform/hardware/keymatrix/scanner")
add_executable(panel15_test panel15_test.cpp)
target_compile_features(panel15_test PRIVATE cxx_std_20)
target_include_directories(panel15_test PRIVATE "${scanner}" "${lib}/engine/keypad")
add_test(NAME panel15 COMMAND panel15_test)

set(core "${lib}/platform/hardware/keymatrix/core")
add_executable(panel15_input_test panel15_input_test.cpp "${core}/keymatrix_handler_type_varikey_15sbla.cpp")
target_compile_features(panel15_input_test PRIVATE cxx_std_20)
target_include_directories(panel15_input_test PRIVATE "${scanner}" "${core}" "${lib}/engine/keypad")
add_test(NAME panel15_input COMMAND panel15_input_test)

add_executable(panel15_preset_test panel15_preset_test.cpp "${interface}/payload/payload_keypad.cpp")
target_compile_features(panel15_preset_test PRIVATE cxx_std_20)
target_include_directories(panel15_preset_test PRIVATE "${interface}/payload" "${lib}/engine/keypad" "${lib}/pulp")
add_test(NAME panel15_preset COMMAND panel15_preset_test)
