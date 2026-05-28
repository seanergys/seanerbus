// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <seanerbus/internal/message.hpp>

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
class RegisterResolveReqV1
{
public:
    /**
     * Decode a payload as a RegisterResolveReqV1
     */
    RegisterResolveReqV1(std::pair<uint64_t, std::vector<std::byte>> payload) : addr(Uuid(0, 0))
    {
        if (!payload.first == (uint64_t)::MessageTypes::REGISTER_RESOLVE_REQ_V1)
        {
            throw std::runtime_error("Trying to decode a message as a RegisterResolveReqV1 that is "
                                     "not a RegisterResolveReqV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::RegisterResolveReqV1::Reader rd = msg.getRoot<::RegisterResolveReqV1>();

        for (const auto& attr : rd.getName())
        {
            name.emplace(attr.getKey(), attr.getValue());
        }
        addr.u_.lower = rd.getAddress().getLower();
        addr.u_.upper = rd.getAddress().getUpper();
    }

    /**
     * Create a new RegisterResolveReqV1 from the given name.
     */
    explicit RegisterResolveReqV1(std::map<std::string, std::string> name, Address addr)
    : name(name), addr(addr)
    {
    }

    /**
     * Encode the RegisterResolveReqV1 with Cap'n'Proto to send it with the client API.
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::RegisterResolveReqV1::Builder tv_msg = msg.initRoot<::RegisterResolveReqV1>();

        auto attr_bld = tv_msg.initName(name.size());

        int i = 0;
        for (const auto& kv : name)
        {
            attr_bld[i].setKey(kv.first);
            attr_bld[i].setValue(kv.second);

            i++;
        }
        auto msg_addr = tv_msg.initAddress();
        msg_addr.setLower(addr.u_.lower);
        msg_addr.setUpper(addr.u_.upper);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::REGISTER_RESOLVE_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::map<std::string, std::string> name;
    Address addr;
};
} // namespace msg
} // namespace seanerbus
