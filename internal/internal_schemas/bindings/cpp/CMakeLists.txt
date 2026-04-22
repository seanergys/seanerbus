# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus_schemas VERSION 0.0.1)

find_package(PkgConfig)

pkg_check_modules(fmt REQUIRED IMPORTED_TARGET fmt)
pkg_check_modules(capnp REQUIRED IMPORTED_TARGET capnp)

add_library(seanerbus_schemas "src/message.cpp" "src/error_payload.cpp")
target_link_libraries(seanerbus_schemas PkgConfig::fmt PkgConfig::capnp)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/schemas/msg.capnp.c++"
    COMMAND capnp compile 
        -oc++ 
        --src-prefix="${CMAKE_CURRENT_SOURCE_DIR}/../../"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../schemas/msg.capnp"
    COMMENT "Generating Cap'n'Proto files"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(generate_capnp DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/schemas/msg.capnp.c++")
add_dependencies(seanerbus_schemas generate_capnp)

target_sources(seanerbus_schemas PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/schemas/msg.capnp.c++")

target_include_directories(
	seanerbus_schemas PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                           "${CMAKE_CURRENT_BINARY_DIR}")

add_library(Seanerbus::internal_schemas ALIAS seanerbus_schemas)
