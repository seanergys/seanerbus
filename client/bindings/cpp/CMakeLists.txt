# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus_client VERSION 0.0.1)

add_subdirectory(../../../internal/internal_schemas/bindings/cpp internal_schemas)


add_library(seanerbus_client INTERFACE)

add_library(Seanerbus::client ALIAS seanerbus_client)
target_link_libraries(seanerbus_client INTERFACE Seanerbus::internal_schemas)

target_include_directories(
    seanerbus_client INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
