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
class UnregisterResolveReqV1
{
public:
    /**
     * Decode a payload as a UnregisterResolveReqV1
     */
    UnregisterResolveReqV1(std::pair<uint64_t, std::vector<std::byte>> payload) : addr(Uuid(0, 0))
    {
        if (payload.first != (uint64_t)::MessageTypes::UNREGISTER_RESOLVE_REQ_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a UnregisterResolveReqV1 that is "
                "not a UnregisterResolveReqV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::UnregisterResolveReqV1::Reader rd = msg.getRoot<::UnregisterResolveReqV1>();

        addr.u_.lower = rd.getAddress().getLower();
        addr.u_.upper = rd.getAddress().getUpper();
    }

    /**
     * Create a new UnregisterResolveReqV1 from the given addr.
     */
    explicit UnregisterResolveReqV1(Address addr) : addr(addr)
    {
    }

    /**
     * Encode the UnregisterResolveReqV1 with Cap'n'Proto to send it with the client API.
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::UnregisterResolveReqV1::Builder tv_msg = msg.initRoot<::UnregisterResolveReqV1>();

        auto msg_addr = tv_msg.initAddress();
        msg_addr.setLower(addr.u_.lower);
        msg_addr.setUpper(addr.u_.upper);

        auto res = capnp::messageToFlatArray(msg);
        return std::make_pair(
            (uint64_t)::MessageTypes::UNREGISTER_RESOLVE_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    Address addr;
};
} // namespace msg
} // namespace seanerbus
