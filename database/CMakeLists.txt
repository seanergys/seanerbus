# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus_db VERSION 0.0.1)

add_subdirectory(../bindings/cpp client)

add_executable(db src/database.cpp src/log.cpp)

target_link_libraries(db PUBLIC Seanerbus::seanerbus)

target_include_directories(db PRIVATE include)

add_subdirectory(lib/hta)
target_link_libraries(db PUBLIC hta)
