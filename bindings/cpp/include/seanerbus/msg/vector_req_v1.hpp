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
class VectorReqV1
{
public:
    /**
     * Decode a payload as a VectorReqV1
     */
    VectorReqV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::VECTOR_REQ_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a VectorReqV1 that is not a VectorReqV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::VectorReqV1::Reader rd = msg.getRoot<::VectorReqV1>();

        for (const auto& elem : rd.getValue())
        {
            vec.emplace_back(elem);
        }
    }

    /**
     * Create a VectorReqV1 from a std::vector<double>
     */
    explicit VectorReqV1(std::vector<double> vec) : vec(vec)
    {
    }

    /**
     * Output the Cap'n'Proto encoded data, as ingested by the C++ client library
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::VectorReqV1::Builder tv_msg = msg.initRoot<::VectorReqV1>();

        auto value_bld = tv_msg.initValue(vec.size());

        for (int i = 0; i < vec.size(); i++)
        {
            value_bld[i] = vec[i];
        }

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)capnp::schemas::MessageTypes::VECTOR_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::vector<double> vec;
};

} // namespace msg
} // namespace seanerbus
