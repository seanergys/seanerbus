// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/internal/message.hpp>
#include <seanerbus/msg/resolve_req_v1.hpp>
#include <seanerbus/msg/resolve_res_v1.hpp>
#include <seanerbus/msg/reverse_resolve_req_v1.hpp>
#include <seanerbus/msg/reverse_resolve_res_v1.hpp>

#include <map>
#include <string>

namespace seanerbus
{
namespace helpers
{
namespace
{

void resolve_res_fn(seanerbus::client::Connection* conn, seanerbus::Message msg,
                    std::vector<Address>* res)
{
    if (msg.payload.first != (uint64_t)::MessageTypes::RESOLVE_RES_V1)
    {
        std::cerr << "Expected Resolve Response!";
        conn->shutdown();

        return;
    }

    seanerbus::msg::ResolveResV1 response(msg.payload);

    *res = response.addrs;
    conn->shutdown();

    return;
}
} // namespace

inline std::vector<Address> resolve(std::map<std::string, std::string> name)
{
    std::vector<Address> res;
    auto payload = seanerbus::msg::ResolveReqV1(name).to_capnp();

    seanerbus::client::Connection conn("localhost", "5398");

    conn.request(seanerbus::Address::locator(), payload, resolve_res_fn, &res);

    seanerbus::client::SimpleEventLoop loop(conn);
    loop.run();
    return res;
}

namespace
{
void reverse_resolve_res_fn(client::Connection* conn, seanerbus::Message msg,
                            std::map<std::string, std::string>* name)
{
    if (msg.payload.first != (uint64_t)::MessageTypes::REVERSE_RESOLVE_RES_V1)
    {
        std::cerr << "Expected Resolve Response!";
        conn->shutdown();

        return;
    }

    seanerbus::msg::ReverseResolveResV1 response(msg.payload);

    *name = response.name;
    conn->shutdown();

    return;
}
} // namespace

inline std::map<std::string, std::string> reverse_resolve(Address addr)
{
    std::map<std::string, std::string> res;
    auto payload = seanerbus::msg::ReverseResolveReqV1(addr).to_capnp();

    seanerbus::client::Connection conn("localhost", "5398");

    conn.request(seanerbus::Address::locator(), payload, reverse_resolve_res_fn, &res);

    seanerbus::client::SimpleEventLoop loop(conn);
    loop.run();
    return res;
}
} // namespace helpers
} // namespace seanerbus
