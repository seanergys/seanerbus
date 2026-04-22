// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/internal/message.hpp>
#include <seanerbus/msg/flux_lifecycle_event_v1.hpp>
#include <seanerbus/msg/history_req_v1.hpp>
#include <seanerbus/msg/history_res_v1.hpp>
#include <seanerbus/msg/register_resolve_req_v1.hpp>
#include <seanerbus/msg/register_resolve_res_v1.hpp>
#include <seanerbus/msg/resolve_req_v1.hpp>
#include <seanerbus/msg/resolve_res_v1.hpp>
#include <seanerbus/msg/reverse_resolve_req_v1.hpp>
#include <seanerbus/msg/reverse_resolve_res_v1.hpp>

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

TEST_CASE("FluxLifecycleEventV1 enc/dec works")
{
    auto payload = seanerbus::msg::FluxLifecycleEventV1("I'm a test").to_capnp();

    REQUIRE(seanerbus::msg::FluxLifecycleEventV1(payload).str_data == "I'm a test");
}

TEST_CASE("HistoryReqV1 enc/dec works")
{
    auto payload = seanerbus::msg::HistoryReqV1(seanerbus::Address::locator(), 1, 2, 3,
                                                seanerbus::msg::HistoryType::AGGREGATE);

    auto hist_req = seanerbus::msg::HistoryReqV1(payload);

    REQUIRE(hist_req.addr == seanerbus::Address::locator());
    REQUIRE(hist_req.start == 1);
    REQUIRE(hist_req.end == 2);
    REQUIRE(hist_req.interval_max == 3);
    REQUIRE(hist_req.type == seanerbus::msg::HistoryType::AGGREGATE);
}

TEST_CASE("HistoryResV1 enc/dec works")
{
    auto payload1 = seanerbus::msg::HistoryResV1::make_error("I'm an error!").to_capnp();

    seanerbus::msg::HistoryResV1 res1(payload1);

    REQUIRE(!res1.success());

    REQUIRE(res1.get_error() == "I'm an error!");

    REQUIRE_THROWS(res1.get_history());

    auto payload2 =
        seanerbus::msg::HistoryResV1::make_raw({ 0, 1, 2, 3 }, { 10.0, 9.0, 8.0 }).to_capnp();

    seanerbus::msg::HistoryResV1 res2(payload2);

    REQUIRE(res2.success());

    REQUIRE_THROWS(res2.get_error());
    REQUIRE(res2.get_history().is_raw());

    REQUIRE_THROWS(res2.get_history().get_aggr());

    REQUIRE(res2.get_history().get_raw() == std::vector<double>{ 10.0, 9.0, 8.0 });
    REQUIRE(res2.get_history().get_time_delta() == std::vector<int64_t>{ 0, 1, 2, 3 });

    auto aggr1 = seanerbus::msg::Aggregate();
    aggr1.minimum = 1.0;
    aggr1.maximum = 2.0;
    aggr1.sum = 3.0;
    aggr1.count = 4;
    aggr1.integral = 5.0;
    aggr1.activeTime = 6;

    auto aggr2 = seanerbus::msg::Aggregate();

    aggr2.minimum = 11.0;
    aggr2.maximum = 12.0;
    aggr2.sum = 13.0;
    aggr2.count = 14;
    aggr2.integral = 15.0;
    aggr2.activeTime = 16;

    auto payload3 = seanerbus::msg::HistoryResV1::make_aggr({ 1, 2, 3, 4 }, { aggr1, aggr2 });

    seanerbus::msg::HistoryResV1 res3(payload3);

    REQUIRE(res3.success());

    REQUIRE_THROWS(res3.get_error());

    REQUIRE(!res3.get_history().is_raw());

    REQUIRE_THROWS(res3.get_history().get_raw());

    REQUIRE(res3.get_history().get_aggr().size() == 2);

    REQUIRE(res3.get_history().get_time_delta() == std::vector<int64_t>{ 1, 2, 3, 4 });

    REQUIRE(res3.get_history().get_aggr()[0].minimum == 1.0);
    REQUIRE(res3.get_history().get_aggr()[0].maximum == 2.0);
    REQUIRE(res3.get_history().get_aggr()[0].sum == 3.0);
    REQUIRE(res3.get_history().get_aggr()[0].count == 4);
    REQUIRE(res3.get_history().get_aggr()[0].integral == 5.0);
    REQUIRE(res3.get_history().get_aggr()[0].activeTime == 6);
    REQUIRE(res3.get_history().get_aggr()[1].maximum == 12.0);
    REQUIRE(res3.get_history().get_aggr()[1].sum == 13.0);
    REQUIRE(res3.get_history().get_aggr()[1].count == 14);
    REQUIRE(res3.get_history().get_aggr()[1].integral == 15.0);
    REQUIRE(res3.get_history().get_aggr()[1].activeTime == 16);
}
