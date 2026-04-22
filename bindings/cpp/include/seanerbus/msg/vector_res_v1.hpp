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
 * A response type for encoding a vector of float values
 */
class VectorResV1
{
public:
    /**
     * Decode a payload as a VectorResV1
     */
    VectorResV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::VECTOR_RES_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a VectorResV1 that is not a VectorResV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::VectorResV1::Reader rd = msg.getRoot<::VectorResV1>();

        for (const auto& elem : rd.getResults())
        {
            vec.emplace_back(elem);
        }
    }

    /**
     * Create a VectorResV1 from a std::vector<double>
     */
    explicit VectorResV1(std::vector<double> vec) : vec(vec)
    {
    }

    /**
     * Output the Cap'n'Proto encoded data, as ingested by the C++ client library
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::VectorResV1::Builder tv_msg = msg.initRoot<::VectorResV1>();

        auto value_bld = tv_msg.initResults(vec.size());

        for (int i = 0; i < vec.size(); i++)
        {
            value_bld[i] = vec[i];
        }

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)capnp::schemas::MessageTypes::VECTOR_RES_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::vector<double> vec;
};

} // namespace msg
} // namespace seanerbus
