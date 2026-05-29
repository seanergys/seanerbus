// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <string>
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

class FluxLifecycleEventV1
{
public:
    FluxLifecycleEventV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::FLUX_LIFECYCLE_EVENT_V1)
        {
            throw std::runtime_error("Trying to decode a message as a FluxLifecycleEventV1 that is "
                                     "not a FluxLifecycleEventV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::FluxLifecycleEventV1::Reader rd = msg.getRoot<::FluxLifecycleEventV1>();

        str_data = rd.getData();
    }

    explicit FluxLifecycleEventV1(std::string str_data) : str_data(str_data)
    {
    }

    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::FluxLifecycleEventV1::Builder tv_msg = msg.initRoot<::FluxLifecycleEventV1>();

        tv_msg.setData(str_data);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::FLUX_LIFECYCLE_EVENT_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::string str_data;
};

} // namespace msg
} // namespace seanerbus
