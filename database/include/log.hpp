// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>

#include <fmt/format.h>

namespace seanerbus
{
namespace logging

{

enum class LogLevel
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
    DISABLED = 6
};
extern LogLevel log_level;

void set_log_level(LogLevel lvl);

template <typename... T>
void trace(fmt::format_string<T...> fmt, T&&... args)
{
    if (log_level <= LogLevel::TRACE)
    {
        std::cerr << "TRACE: " << fmt::format(fmt, std::forward<T>(args)...) << std::endl;
    }
}

template <typename... T>
void debug(fmt::format_string<T...> fmt, T&&... args)
{
    if (log_level <= LogLevel::DEBUG)
    {
        std::cerr << "DEBUG: " << fmt::format(fmt, std::forward<T>(args)...) << std::endl;
    }
}

template <typename... T>
void info(fmt::format_string<T...> fmt, T&&... args)
{
    if (log_level <= LogLevel::INFO)
    {
        std::cerr << "INFO: " << fmt::format(fmt, std::forward<T>(args)...) << std::endl;
    }
}

template <typename... T>
void warn(fmt::format_string<T...> fmt, T&&... args)
{
    if (log_level <= LogLevel::WARN)
    {
        std::cerr << "WARN: " << fmt::format(fmt, std::forward<T>(args)...) << std::endl;
    }
}

template <typename... T>
void error(fmt::format_string<T...> fmt, T&&... args)
{
    if (log_level <= LogLevel::ERROR)
    {
        std::cerr << "ERROR: " << fmt::format(fmt, std::forward<T>(args)...) << std::endl;
    }
}

template <typename... T>
void fatal(fmt::format_string<T...> fmt, T&&... args)
{
    if (log_level <= LogLevel::FATAL)
    {
        std::cerr << "FATAL: " << fmt::format(fmt, std::forward<T>(args)...) << std::endl;
    }
}
} // namespace logging
} // namespace seanerbus
