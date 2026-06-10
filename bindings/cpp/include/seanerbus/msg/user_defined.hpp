// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <stdexcept>
#include <utility>
#include <vector>

#include <cstddef>
#include <cstdint>

#include <capnp/endian.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <seanerbus/msg/msg.capnp.h>

namespace seanerbus

{
namespace msg
{

/**
 * A request containing a Vector of float values.
 *
 * Useful for encoding tuples of doubles, for example
 */
class UserDefined
{
public:
    /**
     * Decode a payload as a UserDefined
     */
    UserDefined(std::pair<uint64_t, std::vector<std::byte>> payload_in)
    {
        if (payload_in.first < 1ULL << 63)
        {
            throw std::runtime_error("User defined types need to have a payload type > 1 << 63");
        }

        payload = payload_in.second;
        payload_type = payload_in.first;
    }

    /**
     * Output the Cap'n'Proto encoded data, as ingested by the C++ client library
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        return std::make_pair(payload_type, payload);
    }

    uint64_t payload_type;
    std::vector<std::byte> payload;
};

} // namespace msg
} // namespace seanerbus
