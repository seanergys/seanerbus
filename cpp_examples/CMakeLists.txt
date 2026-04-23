# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus_examples VERSION 0.0.1)

add_subdirectory(../client/bindings/cpp client)
add_subdirectory(../msg/bindings/cpp msg)


add_executable(subscribe_test src/subscribe_test.cpp)

target_link_libraries(subscribe_test PUBLIC Seanerbus::client Seanerbus::msg)

add_executable(publish_test src/publish_test.cpp)

target_link_libraries(publish_test PUBLIC Seanerbus::client Seanerbus::msg)

add_executable(request_test src/request_test.cpp)

target_link_libraries(request_test PUBLIC Seanerbus::client Seanerbus::msg)

add_executable(response_test src/response_test.cpp)

target_link_libraries(response_test PUBLIC Seanerbus::client Seanerbus::msg)
