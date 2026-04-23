// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
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

class TimeValueV1
{
public:
    TimeValueV1(std::byte* buf, size_t size)
    {
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(buf), size), msg);

        ::TimeValueV1::Reader rd = msg.getRoot<::TimeValueV1>();

        time = rd.getTime();
        value = rd.getValue();
    }

    explicit TimeValueV1(uint64_t time, double value) : time(time), value(value)
    {
    }

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

    uint64_t time;
    uint64_t value;
};

} // namespace msg
} // namespace seanerbus
