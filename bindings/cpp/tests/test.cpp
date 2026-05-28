// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/helpers/one_shot_resolve.hpp>
#include <seanerbus/msg/resolve_req_v1.hpp>
#include <seanerbus/msg/resolve_res_v1.hpp>
#include <seanerbus/msg/reverse_resolve_req_v1.hpp>
#include <seanerbus/msg/reverse_resolve_res_v1.hpp>

#include "seanerbus/internal/message.hpp"

TEST_CASE("ResolveReqV1 enc/dec works")
{
    auto payload = seanerbus::msg::ResolveReqV1({ { "foo", "bar" }, { "bar", "baz" } }).to_capnp();

    REQUIRE(seanerbus::msg::ResolveReqV1(payload).name ==
            std::map<std::string, std::string>{ { "foo", "bar" }, { "bar", "baz" } });
}

TEST_CASE("ResolveResV1 enc/dec works")
{
    auto payload =
        seanerbus::msg::ResolveResV1({ seanerbus::Address(seanerbus::Uuid(0, 0xdeadbeef)),
                                       seanerbus::Address(seanerbus::Uuid(0xdeadbeef, 0xf00)) })
            .to_capnp();

    REQUIRE(
        seanerbus::msg::ResolveResV1(payload).addrs ==
        std::vector<seanerbus::Address>{ seanerbus::Address(seanerbus::Uuid(0, 0xdeadbeef)),
                                         seanerbus::Address(seanerbus::Uuid(0xdeadbeef, 0xf00)) });
}

TEST_CASE("ReverseResolveReqV1 enc/dec works")
{
    auto payload =
        seanerbus::msg::ReverseResolveReqV1(seanerbus::Address(seanerbus::Uuid(0, 0xdeadbeef)))
            .to_capnp();

    REQUIRE(seanerbus::msg::ReverseResolveReqV1(payload).addr ==
            seanerbus::Address(seanerbus::Uuid(0, 0xdeadbeef)));
}

TEST_CASE("ReverseResolveResV1 enc/dec works")
{
    auto payload =
        seanerbus::msg::ReverseResolveResV1({ { "foo", "bar" }, { "bar", "baz" } }).to_capnp();

    REQUIRE(seanerbus::msg::ReverseResolveResV1(payload).name ==
            std::map<std::string, std::string>{ { "foo", "bar" }, { "bar", "baz" } });
}

TEST_CASE("RegisterResolveReqV1 enc/dec works")
{
    auto payload = seanerbus::msg::RegisterResolveReqV1({ { "foo", "bar" }, { "bar", "baz" } },
                                                        seanerbus::Address(seanerbus::Uuid(42, 24)))
                       .to_capnp();

    REQUIRE(seanerbus::msg::RegisterResolveReqV1(payload).name ==
            std::map<std::string, std::string>{ { "foo", "bar" }, { "bar", "baz" } });
    REQUIRE(seanerbus::msg::RegisterResolveReqV1(payload).addr ==
            seanerbus::Address(seanerbus::Uuid(42, 24)));
}

TEST_CASE("RegisterResolveResV1 enc/dec works")
{
    auto payload = seanerbus::msg::RegisterResolveResV1(false, "I'm not happy").to_capnp();

    REQUIRE(!seanerbus::msg::RegisterResolveResV1(payload).success);
    REQUIRE(seanerbus::msg::RegisterResolveResV1(payload).err_msg == "I'm not happy");
}
