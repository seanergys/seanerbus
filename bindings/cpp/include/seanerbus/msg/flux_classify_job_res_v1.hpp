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

class FluxClassifyJobResV1
{
public:
    FluxClassifyJobResV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::FLUX_CLASSIFY_JOB_RES_V1)
        {
            throw std::runtime_error("Trying to decode a message as a FluxClassifyJobResV1 that is "
                                     "not a FluxClassifyJobResV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::FluxClassifyJobResV1::Reader rd = msg.getRoot<::FluxClassifyJobResV1>();

        str_result = rd.getResult();
    }

    explicit FluxClassifyJobResV1(std::string str_result) : str_result(str_result)
    {
    }

    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::FluxClassifyJobResV1::Builder tv_msg = msg.initRoot<::FluxClassifyJobResV1>();

        tv_msg.setResult(str_result);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::FLUX_CLASSIFY_JOB_RES_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::string str_result;
};

} // namespace msg
} // namespace seanerbus
