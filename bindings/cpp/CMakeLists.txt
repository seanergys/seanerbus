# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(seanerbus VERSION 0.0.1)

find_package(PkgConfig)

set(FMT_INSTALL ON)
add_subdirectory(lib/fmt)

pkg_check_modules(capnp REQUIRED IMPORTED_TARGET capnp)

add_library(seanerbus  "src/internal/message.cpp" "src/internal/error_payload.cpp")

target_link_libraries(seanerbus  PkgConfig::capnp fmt::fmt-header-only)

make_directory("${CMAKE_CURRENT_BINARY_DIR}/seanerbus/internal")
make_directory("${CMAKE_CURRENT_BINARY_DIR}/seanerbus/msg")

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/internal/internal.capnp.c++"
    COMMAND capnp compile 
        "-oc++:seanerbus/internal" 
        --src-prefix="${CMAKE_CURRENT_SOURCE_DIR}/../../internal/internal_schemas/schemas"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../internal/internal_schemas/schemas/internal.capnp"
    COMMENT "Generating Cap'n'Proto files"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(generate_capnp DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/internal/internal.capnp.c++")
add_dependencies(seanerbus generate_capnp)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/msg/msg.capnp.c++"
    COMMAND capnp compile 
        "-oc++:seanerbus/msg"
        --src-prefix="${CMAKE_CURRENT_SOURCE_DIR}/../../msg/schemas"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../msg/schemas/msg.capnp"
        COMMENT "Generating Payload Cap'n'Proto files"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(generate_msg_capnp DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/msg/msg.capnp.c++")
add_dependencies(seanerbus generate_msg_capnp)

target_sources(seanerbus PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/msg/msg.capnp.c++")

target_sources(seanerbus PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/msg/msg.capnp.c++")

target_include_directories(
	seanerbus PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  

    $<INSTALL_INTERFACE:include/>)

add_library(Seanerbus::seanerbus ALIAS seanerbus)


include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/seanerbus.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/seanerbus.pc"
                 @ONLY)


configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SeanerbusConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/SeanerbusConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Seanerbus)

install(
    TARGETS seanerbus
    EXPORT SeanerbusTargets
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/seanerbus
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/seanerbus" # source directory
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING # install only matched files
        PATTERN "*.hpp" # select header files
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/msg/msg.capnp.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/seanerbus/msg)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/seanerbus/internal/internal.capnp.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/seanerbus/internal)

install(EXPORT SeanerbusTargets
    FILE SeanerbusTargets.cmake
    DESTINATION "${CMAKE_INSTALL_DATADIR}/Seanerbus/cmake"
    NAMESPACE Seanerbus::)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/SeanerbusConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/Seanerbus/cmake"
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/seanerbus.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

if(PROJECT_IS_TOP_LEVEL)
    find_package(Catch2 CONFIG)

    if(Catch2_FOUND)
        include(CTest)
        include(Catch)

        add_executable(tests tests/test.cpp)
        
        target_link_libraries(tests PRIVATE Catch2::Catch2WithMain Seanerbus::seanerbus)
        catch_discover_tests(tests)
    endif()
endif()
