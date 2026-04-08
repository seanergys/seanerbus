// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <schemas/msg.capnp.h>
#include <capnp/blob.h>
#include <seanerbus_schemas/message.hpp>
#include <capnp/message.h>
#include <capnp/serialize.h>

namespace seanerbus
{

    Message Message::from_fd_capnp(int fd)
    {
        capnp::MallocMessageBuilder msg;

        capnp::readMessageCopyFromFd(fd, msg);

        ::Message::Reader rd = msg.getRoot<::Message>();
        Message res;

        res.type = rd.getType();

        auto addr_rd = rd.getAddress();
        res.address = Address({ addr_rd.getUpper(), addr_rd.getLower() });
        
        if(rd.hasFrom())
        {
        auto from_rd = rd.getFrom();
         res.from = ConnectionId({ from_rd.getUpper(), from_rd.getLower() });
        }
        if(rd.hasTo())
        {
        auto to_rd = rd.getFrom();
         res.to = ConnectionId({ to_rd.getUpper(), to_rd.getLower() });
        }

        res.payload.payload_type = rd.getPayloadType();
        auto msg_payload = rd.getPayload();

        std::vector<std::byte> payload((std::byte*)msg_payload.begin(),
                                      (std::byte*)msg_payload.end());
        res.payload.payload = payload;

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

    if(to.has_value())
    {
    auto msg_to = sub_msg.initTo();
    msg_to.setUpper(to->u_.upper);
    msg_to.setLower(to->u_.lower);
    }

    if(from.has_value())
    {
    auto msg_from = sub_msg.initFrom();
    msg_from.setUpper(to->u_.upper);
    msg_from.setLower(to->u_.lower);
    }

    sub_msg.setPayload(capnp::Data::Reader((const kj::byte*)payload.payload.data(), payload.payload.size()));
    sub_msg.setPayloadType(payload.payload_type);

    writeMessageToFd(fd, message);

    }
}

