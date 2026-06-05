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
 * A "Hello World" Test response
 */
class RegisterResolveResV1
{
public:
    /**
     * Decode a payload as a RegisterResolveResV1
     */
    RegisterResolveResV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (!payload.first == (uint64_t)::MessageTypes::REGISTER_RESOLVE_RES_V1)
        {
            throw std::runtime_error("Trying to decode something as RegisterResolveResV1 that is "
                                     "not RegisterResolveResV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);
        ::RegisterResolveResV1::Reader rd = msg.getRoot<::RegisterResolveResV1>();

        err_msg = rd.getErrMsg();
        success = rd.getSuccess();
    }

    /**
     * Create a RegisterResolveResV1 from a string message
     */
    explicit RegisterResolveResV1(bool success, std::string err_msg)
    : success(success), err_msg(err_msg)
    {
    }

    /**
     * Serialize the RegisterResolveResV1 as a Cap'n'Proto payload, which can be sent with the
     * Seanerbus Client API
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::RegisterResolveResV1::Builder tv_msg = msg.initRoot<::RegisterResolveResV1>();

        tv_msg.setErrMsg(err_msg);
        tv_msg.setSuccess(success);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::REGISTER_RESOLVE_RES_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    bool success;
    /**
     * The error message if success is not true.
     */
    std::string err_msg;
};

} // namespace msg
} // namespace seanerbus
