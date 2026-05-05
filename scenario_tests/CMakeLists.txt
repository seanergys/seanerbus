# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus_examples VERSION 0.0.1)

add_subdirectory(../client/bindings/cpp client)
add_subdirectory(../msg/bindings/cpp msg)


add_executable(scenario_publisher src/scenario_publisher.cpp)

target_link_libraries(scenario_publisher PUBLIC Seanerbus::client Seanerbus::msg)
