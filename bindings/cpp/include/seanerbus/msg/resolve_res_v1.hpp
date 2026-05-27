// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <seanerbus/internal/message.hpp>

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
class ResolveResV1
{
public:
    /**
     * Decode a payload as a ResolveResV1
     */
    ResolveResV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (payload.first != (uint64_t)::MessageTypes::RESOLVE_RES_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a ResolveResV1 that is not a ResolveResV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::ResolveResV1::Reader rd = msg.getRoot<::ResolveResV1>();

        for (const auto& attr : rd.getMatchingInterfaces())
        {
            addrs.emplace_back(Address(Uuid(attr.getUpper(), attr.getLower())));
        }
    }

    /**
     * Create a new ResolveResV1 from the given name.
     */
    explicit ResolveResV1(std::vector<Address> addrs) : addrs(addrs)
    {
    }

    /**
     * Encode the ResolveResV1 with Cap'n'Proto to send it with the client API.
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::ResolveResV1::Builder tv_msg = msg.initRoot<::ResolveResV1>();

        auto attr_bld = tv_msg.initMatchingInterfaces(addrs.size());

        int i = 0;
        for (const auto& addr : addrs)
        {
            attr_bld[i].setLower(addr.u_.lower);
            attr_bld[i].setUpper(addr.u_.upper);

            i++;
        }
        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::RESOLVE_RES_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::vector<Address> addrs;
};
} // namespace msg
} // namespace seanerbus
