// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <log.hpp>

namespace seanerbus
{

namespace logging
{

LogLevel log_level = LogLevel::INFO;

void set_log_level(LogLevel lvl)
{
    log_level = lvl;
}
} // namespace logging
} // namespace seanerbus
