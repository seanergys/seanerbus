// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <capnp/message.h>

#include <seanerbus/internal/message.hpp>

#include <filesystem>
#include <optional>

#include <capnp/blob.h>
#include <capnp/serialize.h>
#include <seanerbus/internal/internal.capnp.h>

namespace seanerbus
{
std::pair<size_t, Message> Message::from_buf_capnp(kj::ArrayPtr<capnp::word> ptr)
{
    capnp::MallocMessageBuilder msg;

    auto res = initMessageBuilderFromFlatArrayCopy(ptr, msg);

    ::Message::Reader rd = msg.getRoot<::Message>();

    return std::make_pair((res.begin() - ptr.begin()) * sizeof(capnp::word), Message::from_msg(rd));
}

Message Message::from_msg(::Message::Reader& rd)
{
    Message res;

    res.type = rd.getType();

    auto addr_rd = rd.getAddress();
    res.matching_id = rd.getMatchingId();
    res.address = Address({ addr_rd.getUpper(), addr_rd.getLower() });

    if (rd.hasFrom())
    {
        auto from_rd = rd.getFrom();
        res.from = ConnectionId({ from_rd.getUpper(), from_rd.getLower() });
    }
    if (rd.hasTo())
    {
        auto to_rd = rd.getFrom();
        res.to = ConnectionId({ to_rd.getUpper(), to_rd.getLower() });
    }

    auto msg_payload = rd.getPayload();

    std::vector<std::byte> payload((std::byte*)msg_payload.begin(), (std::byte*)msg_payload.end());
    res.payload = std::make_pair(rd.getPayloadType(), payload);

    return res;
}

void Message::to_fd_capnp(int fd)
{
    capnp::MallocMessageBuilder message;
    ::Message::Builder sub_msg = message.initRoot<::Message>();

    sub_msg.setType(type);
    auto msg_uuid = sub_msg.initAddress();
    msg_uuid.setUpper(this->address.u_.upper);
    msg_uuid.setLower(address.u_.lower);

    if (to.has_value())
    {
        auto msg_to = sub_msg.initTo();
        msg_to.setUpper(to->u_.upper);
        msg_to.setLower(to->u_.lower);
    }

    if (from.has_value())
    {
        auto msg_from = sub_msg.initFrom();
        msg_from.setUpper(to->u_.upper);
        msg_from.setLower(to->u_.lower);
    }

    sub_msg.setMatchingId(this->matching_id);

    sub_msg.setPayload(
        capnp::Data::Reader((const kj::byte*)payload.second.data(), payload.second.size()));
    sub_msg.setPayloadType(payload.first);

    writeMessageToFd(fd, message);
}

Message Message::subscribe(Address addr)
{
    Message res;

    res.type = ::Message::MessageType::SUBSCRIBE;
    res.to = std::nullopt;
    res.from = std::nullopt;
    res.address = addr;
    res.payload = { 0, std::vector<std::byte>() };
    return res;
}

Message Message::publish(Address addr, std::pair<uint64_t, std::vector<std::byte>> payload)
{
    Message res;

    res.type = ::Message::MessageType::PUBLISH;
    res.to = std::nullopt;
    res.from = std::nullopt;
    res.address = addr;
    res.payload = payload;
    return res;
}

Message Message::request(Address addr, std::pair<uint64_t, std::vector<std::byte>> payload)
{
    Message res;

    res.type = ::Message::MessageType::REQUEST;
    res.matching_id = rand();
    res.to = std::nullopt;
    res.from = std::nullopt;
    res.address = addr;
    res.payload = payload;
    return res;
}

Message Message::response(Message msg, std::pair<uint64_t, std::vector<std::byte>> payload)
{
    Message res;

    res.type = ::Message::MessageType::RESPONSE;
    res.to = msg.from;
    res.matching_id = msg.matching_id;
    res.from = std::nullopt;
    res.address = msg.address;
    res.payload = payload;

    return res;
}

Message Message::register_endpoint(Address addr)
{
    Message res;

    res.type = ::Message::MessageType::REGISTER;
    res.to = std::nullopt;
    res.from = std::nullopt;
    res.address = addr;
    res.payload = { 0, std::vector<std::byte>() };

    return res;
}
} // namespace seanerbus
