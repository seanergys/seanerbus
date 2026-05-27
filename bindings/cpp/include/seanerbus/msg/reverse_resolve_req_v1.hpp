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
class ReverseResolveReqV1
{
public:
    /**
     * Decode a payload as a ReverseResolveReqV1
     */
    ReverseResolveReqV1(std::pair<uint64_t, std::vector<std::byte>> payload) : addr(Uuid(0, 0))
    {
        if (payload.first != (uint64_t)::MessageTypes::REVERSE_RESOLVE_REQ_V1)
        {
            throw std::runtime_error("Trying to decode a message as a ReverseResolveReqV1 that is "
                                     "not a ReverseResolveReqV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::ReverseResolveReqV1::Reader rd = msg.getRoot<::ReverseResolveReqV1>();

        addr.u_.lower = rd.getAddress().getLower();
        addr.u_.upper = rd.getAddress().getUpper();
    }

    /**
     * Create a new ReverseResolveReqV1 from the given addr.
     */
    explicit ReverseResolveReqV1(Address addr) : addr(addr)
    {
    }

    /**
     * Encode the ReverseResolveReqV1 with Cap'n'Proto to send it with the client API.
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::ReverseResolveReqV1::Builder tv_msg = msg.initRoot<::ReverseResolveReqV1>();

        auto msg_addr = tv_msg.initAddress();
        msg_addr.setLower(addr.u_.lower);
        msg_addr.setUpper(addr.u_.upper);

        auto res = capnp::messageToFlatArray(msg);
        return std::make_pair(
            (uint64_t)::MessageTypes::REVERSE_RESOLVE_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    Address addr;
};
} // namespace msg
} // namespace seanerbus
