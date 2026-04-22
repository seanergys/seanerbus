// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <capnp/endian.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <cstddef>
#include <cstdint>
#include <vector>

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

    std::vector<std::byte> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::TimeValueV1::Builder tv_msg = msg.initRoot<::TimeValueV1>();

        tv_msg.setTime(time);
        tv_msg.setValue(value);

        auto res = capnp::messageToFlatArray(msg);

        return std::vector<std::byte>(res.begin(), res.end());
    }

    uint64_t time;
    uint64_t value;
};

} // namespace msg
} // namespace seanerbus
