// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <string>
#include <vector>

#include <cstddef>

#include <schemas/internal.capnp.h>
namespace seanerbus
{
struct ErrorPayload
{
    static ErrorPayload from_vec(std::vector<std::byte>);
    std::vector<std::byte> to_vec();
    ::ErrorPayload::ErrorKind kind;
    std::string str;
};
}
