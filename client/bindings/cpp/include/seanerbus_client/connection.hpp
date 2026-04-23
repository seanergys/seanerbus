// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <seanerbus_schemas/message.hpp>

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
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

    template <class invoke_tuple>
    static void* executor(Connection& conn, Message msg, void* arg)
    {
        invoke_tuple* func_arg = static_cast<invoke_tuple*>(arg);

        // Tuple is unpacked here. Don't ask how.
        using indices = std::make_index_sequence<std::tuple_size<invoke_tuple>::value - 1>;
        executor_helper(conn, msg, *func_arg, indices{});

        return nullptr;
    }

    template <class F, class... Args, size_t... Indices>
    static void executor_helper(Connection& conn, Message& msg, std::tuple<F, Args...>& t,
                                std::index_sequence<Indices...> is)
    {
        std::invoke(std::move(std::get<0>(t)), conn, msg, std::move(std::get<Indices + 1>(t))...);
    }

    void publish(Address publish_to, std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        auto msg = seanerbus::Message::publish(publish_to, payload);
        msg.to_fd_capnp(fd_);
    }

    template <class F, class... Args>
    void subscribe(Address subscribe_to, F&& f, Args&&... args)
    {
        auto msg = seanerbus::Message::subscribe(subscribe_to);

        using invoke_tuple = std::tuple<F, Args...>;

        msg.to_fd_capnp(fd_);

        sub_cbs_.emplace(std::piecewise_construct, std::forward_as_tuple(subscribe_to),
                         std::forward_as_tuple(executor<invoke_tuple>));

        void* data = std::unique_ptr<invoke_tuple>(
                         new invoke_tuple(invoke_tuple(std::move(f), std::forward<Args>(args)...)))
                         .release();
        sub_userdata_.emplace(subscribe_to, data);

        deleters_.emplace(std::piecewise_construct, std::forward_as_tuple(data),
                          std::forward_as_tuple(deleter<invoke_tuple>));
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

    Message request(Address address, std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        auto msg = seanerbus::Message::request(address, payload);

        msg.to_fd_capnp(fd_);

        auto ret_msg = seanerbus::Message::from_fd_capnp(fd_);

        if (ret_msg.type != ::Message::MessageType::RESPONSE)
        {
            throw std::runtime_error("Got something else than a response for a request");
        }

        return ret_msg;
    }

    void respond_to(Message msg, std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        auto res_msg = seanerbus::Message::response(msg, payload);
        res_msg.to_fd_capnp(fd_);
    }

    template <class F, class... Args>
    void register_endpoint(Address addr, F f, Args... args)
    {
        auto reg_msg = seanerbus::Message::register_endpoint(addr);
        reg_msg.to_fd_capnp(fd_);

        using invoke_tuple = std::tuple<F, Args...>;

        req_cbs_.emplace(std::piecewise_construct, std::forward_as_tuple(addr),
                         std::forward_as_tuple(executor<invoke_tuple>));

        void* data = std::unique_ptr<invoke_tuple>(
                         new invoke_tuple(invoke_tuple(std::move(f), std::forward<Args>(args)...)))
                         .release();
        req_userdata_.emplace(addr, data);

        deleters_.emplace(data, deleter<invoke_tuple>);
    }

    ~Connection()
    {
        for (auto& elem : deleters_)
        {
            elem.second(elem.first);
        }
        close(fd_);
    }

    template <class invoke_tuple>
    static void deleter(void* arg)
    {
        std::unique_ptr<invoke_tuple>{ (invoke_tuple*)arg };
    }

private:
    int fd_ = -1;

    std::map<Address, void* (*)(Connection&, Message, void*)> sub_cbs_;
    std::map<Address, void*> sub_userdata_;
    std::map<Address, void* (*)(Connection&, Message, void*)> req_cbs_;
    std::map<Address, void*> req_userdata_;
    std::map<void*, void (*)(void*)> deleters_;
};
} // namespace client
} // namespace seanerbus
