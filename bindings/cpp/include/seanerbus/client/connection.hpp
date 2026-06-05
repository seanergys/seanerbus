// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <seanerbus/internal/error_payload.hpp>
#include <seanerbus/internal/message.hpp>

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <cerrno>

#include <kj/exception.h>
#include <seanerbus/internal/internal.capnp.h>

extern "C"
{
#include <fcntl.h>
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
/**
 * A single connection to the data-plane.
 *
 * One program can hold multiple independent connections to the same data-plane.
 */
class Connection
{
public:
    /**
     * Creates a new connection to the data-plane.
     *
     * By default, the server listens at localhost:5398.
     *
     * @param address The hostname to connect to (e.g. "localhost" or "10.23.31.12")
     * @param port The port to connect to (5398)
     */
    Connection(std::string address, std::string port)
    {
        buffer_ = reinterpret_cast<std::byte*>(malloc(1024));
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
            throw std::runtime_error("Could not resolve SeanerBUS address with getaddrinfo");
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
            throw std::runtime_error("Could not open connection to SeanerBUS!");
        }
        fd_ = sfd;
        fcntl(fd_, F_SETFL, O_NONBLOCK);
    }

    /**
     * Publish a single payload to the given address.
     *
     * @param publish_to Address to publish to
     * @param payload An encoded payload, as created by the C++ payload libraries to_capnp()
     * function.
     */
    void publish(Address publish_to, std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        auto msg = seanerbus::Message::publish(publish_to, payload);
        msg.to_fd_capnp(fd_);
    }

    /**
     * Tries to read some more data from the underlying connection.
     *
     * If a full Message was received, the received message will be forwarded to the appropriate
     * callback.
     */
    void try_read()
    {
        int res = recv(fd_, buffer_ + cursor_, buffer_size_ - cursor_, 0);

        if (res == 0)
        {
            if (cursor_ != 0)
            {
                throw std::runtime_error("Connection shut down with data left to read!");
            }
            shutdown();
        }
        if (res == buffer_size_ - cursor_)
        {
            buffer_ = reinterpret_cast<std::byte*>(realloc(buffer_, buffer_size_ * 2));
            buffer_size_ = buffer_size_ * 2;
        }
        cursor_ += res;

        try
        {
            while (true)
            {

                auto res = Message::from_buf_capnp(
                    kj::ArrayPtr(reinterpret_cast<capnp::word*>(buffer_), cursor_));
                cursor_ = cursor_ - res.first;
                handle(res.second);
                memmove(buffer_, buffer_ + res.first, buffer_size_ - res.first);
            }
        }
        catch (kj::Exception& e)
        {
        }
    }

    /**
     * Returns the file descriptor associated with this connection.
     * This can then be passed into poll, for example.
     */
    int get_fd()
    {
        return fd_;
    }

    /**
     * Subscribe to messages published on the given address.
     * When the Connection receives a publish message for this address, it will call the function f.
     *
     * The client  will start receiving message from the subscribed addresses immediately.
     * The messages are, however, only processed once main_loop() is called.
     *
     * The function f must always take a Connection& and a Message as a first and second argument.
     * the additional arguments to subscribe() are added to it after the Connection& and Message
     * argument:
     *
     * Without any further arguments:
     *
     *      void func_one(Connection&, Message);
     *      subscribe(some_addr, func_one);
     *
     * When a published message for some_addr is received, func_one is called as:
     *
     *      func_one(*this, received_message);
     *
     * Alternatively if subscribe is given an additional integer  as an argument:
     *
     *      int foobar;
     *      void func_two(Connection, Message, int);
     *      subscribe(some_other_addr, func_two, foobar);
     *
     * This calls (on receive of a message to some_other_addr):
     *
     *      func_two(*this, received_message, foobar)
     *
     *
     * For more information, see cpp_examples/src/subscribe_test.cpp
     *
     * @param subscribe_to Address to subscribe to
     * @param f callback to call when a message from address subscribe_to appears.
     * @param args 0 to n additional arguments to the function f as described above.
     */
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

    /**
     * Send a request to address with the given payload.
     *
     * The callback f and the additional args behave exactly as in subscribe()
     * and are called on receive of the response.
     *
     * See: cpp_examples/src/request_test.cpp for an example.
     *
     * @param address The address to send the request to
     * @param payload The payload to send with the request
     * @param f The function to call when a request comes in.
     * @param args The 0..n arguments forwarded to the function.
     */
    template <class F, class... Args>
    void request(Address address, std::pair<uint64_t, std::vector<std::byte>> payload, F&& f,
                 Args&&... args)
    {
        auto msg = seanerbus::Message::request(address, payload);

        using invoke_tuple = std::tuple<F, Args...>;

        res_cbs_.emplace(std::piecewise_construct, std::forward_as_tuple(msg.matching_id),
                         std::forward_as_tuple(executor<invoke_tuple>));

        void* data = std::unique_ptr<invoke_tuple>(
                         new invoke_tuple(invoke_tuple(std::move(f), std::forward<Args>(args)...)))
                         .release();
        res_userdata_.emplace(msg.matching_id, data);

        deleters_.emplace(data, deleter<invoke_tuple>);

        msg.to_fd_capnp(fd_);
    }

    /**
     * Takes an incoming request and responds to it with payload.
     *
     * @param msg The msg to which we are responding.
     * @param payload The payload to send with the response.
     *
     */
    void respond_to(Message msg, std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        auto res_msg = seanerbus::Message::response(msg, payload);
        res_msg.to_fd_capnp(fd_);
    }

    /**
     * Registers a req/res service at address addr.
     *
     * When another communication partner on the data-plane makes a request to address, the
     * server will forward this to the client that has registered it.
     *
     * The callback f and the additional args behave exactly as in subscribe()
     *
     * See: cpp_examples/src/response_test.cpp for an example.
     *
     * @param addr The addr to register the request service to.
     * @param f The function to call when a request comes in.
     * @param args The 0..n arguments forwarded to the function.
     */
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

    Connection(Connection&) = delete;
    Connection(Connection&&) = delete;

    Connection& operator=(Connection&) = delete;
    Connection& operator=(Connection&&) = delete;

    ~Connection()
    {
        for (auto& elem : deleters_)
        {
            elem.second(elem.first);
        }
        close(fd_);
        free(buffer_);
    }

    /**
     * Signals back to the Connection that the user of the connection wants to shut it down.
     */
    void shutdown()
    {
        shutdown_ = true;
    }

    /**
     * Checks if the user of the connection has requested a shutdown.
     *
     * After calling try_read(), the event-loop code should check if shutdown was requested for this
     * Connection and handle it accordingly.
     */
    bool is_shutdown()
    {
        return shutdown_;
    }

private:
    template <class invoke_tuple>
    static void deleter(void* arg)
    {
        std::unique_ptr<invoke_tuple>{ (invoke_tuple*)arg };
    }

    void handle(seanerbus::Message msg)
    {
        switch (msg.type)
        {
        case ::Message::MessageType::PUBLISH:
            sub_cbs_.at(msg.address)(*this, msg, sub_userdata_.at(msg.address));
            break;
        case ::Message::MessageType::REQUEST:
            req_cbs_.at(msg.address)(*this, msg, req_userdata_.at(msg.address));
            break;
        case ::Message::MessageType::RESPONSE:
            res_cbs_.at(msg.matching_id)(*this, msg, res_userdata_.at(msg.matching_id));
            break;
        case ::Message::MessageType::ERROR:
        {
            auto err = seanerbus::ErrorPayload::from_vec(msg.payload.second);
            throw std::runtime_error(err.str);
        }
        default:
            throw std::runtime_error("Unknown msg type!");
        }
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
        std::invoke(std::move(std::get<0>(t)), &conn, msg, std::move(std::get<Indices + 1>(t))...);
    }

    int fd_ = -1;

    std::map<Address, void* (*)(Connection&, Message, void*)> sub_cbs_;
    std::map<Address, void*> sub_userdata_;
    std::map<Address, void* (*)(Connection&, Message, void*)> req_cbs_;
    std::map<Address, void*> req_userdata_;
    std::map<uint64_t, void* (*)(Connection&, Message, void*)> res_cbs_;
    std::map<uint64_t, void*> res_userdata_;
    std::map<void*, void (*)(void*)> deleters_;
    std::byte* buffer_ = nullptr;
    size_t cursor_ = 0;
    size_t buffer_size_ = 1024;
    bool shutdown_ = false;
};
} // namespace client
} // namespace seanerbus
