# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus_msg VERSION 0.0.1)

find_package(PkgConfig)

pkg_check_modules(fmt REQUIRED IMPORTED_TARGET fmt)
pkg_check_modules(capnp REQUIRED IMPORTED_TARGET capnp)

add_library(seanerbus_msg INTERFACE)
target_include_directories(
    seanerbus_msg INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
                           "${CMAKE_CURRENT_BINARY_DIR}")
add_library(Seanerbus::msg ALIAS seanerbus_msg)
target_link_libraries(seanerbus_msg INTERFACE PkgConfig::capnp)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/schemas/msg.capnp.c++"
    COMMAND capnp compile 
        -oc++ 
        --src-prefix="${CMAKE_CURRENT_SOURCE_DIR}/../../"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../schemas/msg.capnp"
    COMMENT "Generating Cap'n'Proto files"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(generate_msg_capnp DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/schemas/msg.capnp.c++")
add_dependencies(seanerbus_msg generate_msg_capnp)

target_sources(seanerbus_msg PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/schemas/msg.capnp.c++")


