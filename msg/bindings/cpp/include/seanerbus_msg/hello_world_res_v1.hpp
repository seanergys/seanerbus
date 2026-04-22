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
#include <schemas/msg.capnp.h>

namespace seanerbus

{
namespace msg
{

class HelloWorldResV1
{
public:
    HelloWorldResV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::HELLO_WORLD_RES_V1)
        {
            throw std::runtime_error(
                "Trying to decode someting as HelloWorldResV1 that is not HelloWorldResV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);
        ::HelloWorldResV1::Reader rd = msg.getRoot<::HelloWorldResV1>();

        str_msg = rd.getMsg();
    }

    explicit HelloWorldResV1(std::string str_msg) : str_msg(str_msg)
    {
    }

    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::HelloWorldResV1::Builder tv_msg = msg.initRoot<::HelloWorldResV1>();

        tv_msg.setMsg(str_msg);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::HELLO_WORLD_RES_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    std::string str_msg;
};

} // namespace msg
} // namespace seanerbus
