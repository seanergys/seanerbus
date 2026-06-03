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

class FluxClassifyJobReqV1
{
public:
    FluxClassifyJobReqV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::FLUX_CLASSIFY_JOB_REQ_V1)
        {
            throw std::runtime_error("Trying to decode a message as a FluxClassifyJobReqV1 that is "
                                     "not a FluxClassifyJobReqV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::FluxClassifyJobReqV1::Reader rd = msg.getRoot<::FluxClassifyJobReqV1>();

        str_jobspec = rd.getJobspec();
    }

    explicit FluxClassifyJobReqV1(std::string str_jobspec) : str_jobspec(str_jobspec)
    {
    }

    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::FluxClassifyJobReqV1::Builder tv_msg = msg.initRoot<::FluxClassifyJobReqV1>();

        tv_msg.setJobspec(str_jobspec);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::FLUX_CLASSIFY_JOB_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::string str_jobspec;
};

} // namespace msg
} // namespace seanerbus
