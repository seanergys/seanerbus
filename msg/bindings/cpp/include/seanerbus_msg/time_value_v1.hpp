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
#include <schemas/msg.capnp.h>

namespace seanerbus

{
namespace msg
{

/**
 * A pair of a timestamp and a double value
 */
class TimeValueV1
{
public:
    /**
     * Decode a payload as a TimeValueV1
     */
    TimeValueV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::TIME_VALUE_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a TimeValueV1 that is not a TimeValueV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::TimeValueV1::Reader rd = msg.getRoot<::TimeValueV1>();

        time = rd.getTime();
        value = rd.getValue();
    }

    /**
     * Create a TimeValueV1 from a timestamp and value
     */
    explicit TimeValueV1(uint64_t time, double value) : time(time), value(value)
    {
    }

    /**
     * Output the Cap'n'Proto encoded data, as ingestecd by the C++ client library
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::TimeValueV1::Builder tv_msg = msg.initRoot<::TimeValueV1>();

        tv_msg.setTime(time);
        tv_msg.setValue(value);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)capnp::schemas::MessageTypes_b06630b2cb0c9ce8::TIME_VALUE_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    /**
     * A timestamp
     */
    uint64_t time;
    /**
     * A value
     */
    double value;
};

} // namespace msg
} // namespace seanerbus
