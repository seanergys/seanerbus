// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <map>
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
 * A "Hello World" Test request
 */
class ResolveReqV1
{
public:
    /**
     * Decode a payload as a ResolveReqV1
     */
    ResolveReqV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (payload.first != (uint64_t)::MessageTypes::RESOLVE_REQ_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a ResolveReqV1 that is not a ResolveReqV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::ResolveReqV1::Reader rd = msg.getRoot<::ResolveReqV1>();

        for (const auto& attr : rd.getAttributes())
        {
            name.emplace(attr.getKey(), attr.getValue());
        }
    }

    /**
     * Create a new ResolveReqV1 from the given name.
     */
    explicit ResolveReqV1(std::map<std::string, std::string> name) : name(name)
    {
    }

    /**
     * Encode the ResolveReqV1 with Cap'n'Proto to send it with the client API.
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::ResolveReqV1::Builder tv_msg = msg.initRoot<::ResolveReqV1>();

        auto attr_bld = tv_msg.initAttributes(name.size());

        int i = 0;
        for (const auto& kv : name)
        {
            attr_bld[i].setKey(kv.first);
            attr_bld[i].setValue(kv.second);

            i++;
        }
        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::RESOLVE_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::map<std::string, std::string> name;
};
} // namespace msg
} // namespace seanerbus
