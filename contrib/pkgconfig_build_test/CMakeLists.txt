# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus_examples VERSION 0.0.1)

find_package(PkgConfig)

pkg_check_modules(seanerbus REQUIRED IMPORTED_TARGET seanerbus)

add_executable(subscribe_test src/subscribe_test.cpp)

target_link_libraries(subscribe_test PUBLIC PkgConfig::seanerbus)
