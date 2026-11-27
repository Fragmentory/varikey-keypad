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
