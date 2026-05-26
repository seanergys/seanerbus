// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <stdexcept>
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

/**
 * A "Hello World" Test request
 */
class HelloWorldReqV1
{
public:
    /**
     * Decode a payload as a HelloWorldReqV1
     */
    HelloWorldReqV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (payload.first != (uint64_t)::MessageTypes::HELLO_WORLD_REQ_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a HelloWorldRegV1 that is not a HelloWorldRegV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::HelloWorldReqV1::Reader rd = msg.getRoot<::HelloWorldReqV1>();

        str_msg = rd.getMsg();
    }

    /**
     * Create a new HelloWorldReqV1 from the given string.
     */
    explicit HelloWorldReqV1(std::string str_msg) : str_msg(str_msg)
    {
    }

    /**
     * Encode the HelloWorldReqV1 with Cap'n'Proto to send it with the client API.
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::HelloWorldReqV1::Builder tv_msg = msg.initRoot<::HelloWorldReqV1>();

        tv_msg.setMsg(str_msg);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::HELLO_WORLD_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    /**
     * The string in the "Hello World" request.
     */
    std::string str_msg;
};

} // namespace msg
} // namespace seanerbus
