// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus/helpers/resolve.hpp>

#include <seanerbus/msg/helpers.hpp>

#include <stdexcept>

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

std::vector<Address> resolve(client::Connection& conn, std::map<std::string, std::string> name)
{
    try
    {
        std::vector<Address> res;
        auto payload = seanerbus::msg::ResolveReqV1(name).to_capnp();

        conn.request(seanerbus::Address::locator(), payload, resolve_res_fn, &res);

        seanerbus::client::SimpleEventLoop loop(conn);
        loop.run();
        conn.reset_shutdown();
        return res;
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(
            fmt::format("Failed to resolve {}: {}", msg::name_map_to_str(name), e.what()));
    }
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

std::map<std::string, std::string> reverse_resolve(client::Connection& conn, Address addr)
{
    std::map<std::string, std::string> res;
    auto payload = seanerbus::msg::ReverseResolveReqV1(addr).to_capnp();

    conn.request(seanerbus::Address::locator(), payload, reverse_resolve_res_fn, &res);

    seanerbus::client::SimpleEventLoop loop(conn);
    loop.run();
    conn.reset_shutdown();
    return res;
}

void register_resolve_res_fn(client::Connection* conn, seanerbus::Message msg)
{
    if (msg.payload.first != (uint64_t)::MessageTypes::REGISTER_RESOLVE_RES_V1)
    {
        std::cerr << "Expected Register Resolve Response!";
        conn->shutdown();

        return;
    }

    seanerbus::msg::RegisterResolveResV1 response(msg.payload);

    conn->shutdown();

    if (!response.success)
    {
        throw std::runtime_error("registering address with name service failed: " +
                                 response.err_msg);
    }
}

void register_resolve(client::Connection& conn, std::map<std::string, std::string> name,
                      Address addr)
{
    try
    {
        auto payload = seanerbus::msg::RegisterResolveReqV1(name, addr).to_capnp();

        conn.request(seanerbus::Address::locator(), payload, register_resolve_res_fn);

        seanerbus::client::SimpleEventLoop loop(conn);
        loop.run();
        conn.reset_shutdown();
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(fmt::format("Failed to register name {} for address {}: {}",
                                             msg::name_map_to_str(name), addr.as_string(),
                                             e.what()));
    }
}

void unregister_resolve_res_fn(client::Connection* conn, seanerbus::Message msg)
{
    if (msg.payload.first != (uint64_t)::MessageTypes::UNREGISTER_RESOLVE_RES_V1)
    {
        std::cerr << "Expected Register Resolve Response!";
        conn->shutdown();

        return;
    }

    seanerbus::msg::UnregisterResolveResV1 response(msg.payload);

    conn->shutdown();

    if (!response.success)
    {
        throw std::runtime_error("registering address with name service failed: " +
                                 response.err_msg);
    }
}

void unregister_resolve(client::Connection& conn, Address addr)
{
    auto payload = seanerbus::msg::UnregisterResolveReqV1(addr).to_capnp();

    conn.request(seanerbus::Address::locator(), payload, unregister_resolve_res_fn);

    seanerbus::client::SimpleEventLoop loop(conn);
    loop.run();
    conn.reset_shutdown();
}
} // namespace helpers
} // namespace seanerbus
