// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <seanerbus_schemas/message.hpp>

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <schemas/internal.capnp.h>

extern "C"
{
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
}

namespace seanerbus
{

namespace client
{
class Connection
{
public:
    using subscribe_cb = void (*)(Connection&, Message, void*);

    Connection(std::string address, std::string port)
    {
        int sfd, s;
        struct addrinfo hints;
        struct addrinfo *result, *rp;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        hints.ai_flags = 0;
        hints.ai_protocol = 0; /* Any protocol */

        s = getaddrinfo(address.c_str(), port.c_str(), &hints, &result);
        if (s != 0)
        {
            throw std::runtime_error("Could not resolve with getaddrinfo");
        }

        for (rp = result; rp != NULL; rp = rp->ai_next)
        {
            sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sfd == -1)
                continue;

            if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
                break; /* Success */

            close(sfd);
        }

        freeaddrinfo(result); /* No longer needed */

        if (rp == NULL)
        { /* No address succeeded */
            throw std::runtime_error("Could not open a fitting socket!");
        }
        fd_ = sfd;
    }

    void publish(Address publish_to, uint64_t payload_type, std::vector<std::byte> payload)
    {
        auto msg = seanerbus::Message::publish(publish_to, payload_type, payload);
        msg.to_fd_capnp(fd_);
    }

    void subscribe(Address subscribe_to, subscribe_cb sub, void* userdata)
    {
        auto msg = seanerbus::Message::subscribe(subscribe_to);

        msg.to_fd_capnp(fd_);

        sub_cbs_.emplace(subscribe_to, sub);
        sub_userdata_.emplace(subscribe_to, userdata);
    }

    void main_loop()
    {
        while (true)
        {
            auto msg = seanerbus::Message::from_fd_capnp(fd_);

            switch (msg.type)
            {
            case ::Message::MessageType::PUBLISH:
                sub_cbs_.at(msg.address)(*this, msg, sub_userdata_.at(msg.address));
                break;
            case ::Message::MessageType::REQUEST:
                req_cbs_.at(msg.address)(*this, msg, req_userdata_.at(msg.address));
                break;
            default:
                throw std::runtime_error("Unknown msg type!");
            }
        }
    }

    Message request(Address address, uint64_t payload_type, std::vector<std::byte> payload)
    {
        auto msg = seanerbus::Message::request(address, payload_type, payload);

        msg.to_fd_capnp(fd_);

        auto ret_msg = seanerbus::Message::from_fd_capnp(fd_);

        if (ret_msg.type != ::Message::MessageType::RESPONSE)
        {
            throw std::runtime_error("Got something else than a response for a request");
        }

        return ret_msg;
    }

    void respond_to(Message msg, uint64_t payload_type, std::vector<std::byte> payload)
    {
        auto res_msg = seanerbus::Message::response(msg, payload_type, payload);
        res_msg.to_fd_capnp(fd_);
    }

    void register_endpoint(Address addr, subscribe_cb cb, void* userdata)
    {
        auto reg_msg = seanerbus::Message::register_endpoint(addr);
        reg_msg.to_fd_capnp(fd_);

        req_cbs_.emplace(addr, cb);
        req_userdata_.emplace(addr, userdata);
    }

    ~Connection()
    {
        close(fd_);
    }

private:
    int fd_ = -1;

    std::map<Address, subscribe_cb> sub_cbs_;
    std::map<Address, void*> sub_userdata_;
    std::map<Address, subscribe_cb> req_cbs_;
    std::map<Address, void*> req_userdata_;
};
} // namespace client
} // namespace seanerbus
