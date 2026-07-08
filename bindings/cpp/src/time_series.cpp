// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus/helpers/time_series.hpp>

#include <stdexcept>

#include "seanerbus/msg/helpers.hpp"
#include "seanerbus/msg/history_res_v1.hpp"
#include "seanerbus/msg/msg.capnp.h"

namespace seanerbus
{
namespace helpers
{

void get_timeline_res(seanerbus::client::Connection& conn, seanerbus::Message msg,
                      msg::History* hist)
{
    if (msg.payload.first != (uint64_t)MessageTypes::HISTORY_RES_V1)
    {
        throw std::runtime_error(
            fmt::format("Expected HistoryResV1 as response to HistoryReqV1, got {} instead",
                        msg::payload_type_to_str(msg.payload.first)));
    }

    auto res = msg::HistoryResV1(msg.payload);

    if (!res.success())
    {
        throw std::runtime_error(fmt::format(
            "Requesting Timeline data from Time-Series Database failed: {}", res.get_error()));
    }

    *hist = res.get_history();
    conn.shutdown();
}

msg::History get_timeline(client::Connection& conn, Address addr, uint64_t start, uint64_t end,
                          uint64_t interval_max)
{
    auto db_addr = resolve(conn, { { "service", "time_series_db" } });

    if (db_addr.size() != 1)
    {
        throw std::runtime_error(
            "Could not resolve address of time-series DB service, is it running?");
    }

    msg::History hist;

    conn.request(db_addr[0],
                 msg::HistoryReqV1(addr, start, end, interval_max, msg::HistoryType::FLEX_TIMELINE)
                     .to_capnp(),
                 get_timeline_res, &hist);
    client::SimpleEventLoop sel(conn);

    sel.run();

    conn.reset_shutdown();
    return hist;
}

void get_aggregate_res(seanerbus::client::Connection& conn, seanerbus::Message msg,
                       msg::Aggregate* aggr)
{
    if (msg.payload.first != (uint64_t)MessageTypes::HISTORY_RES_V1)
    {
        throw std::runtime_error(
            fmt::format("Expected HistoryResV1 as response to HistoryReqV1, got {} instead",
                        msg::payload_type_to_str(msg.payload.first)));
    }

    auto res = msg::HistoryResV1(msg.payload);

    if (!res.success())
    {
        throw std::runtime_error(fmt::format(
            "Requesting Timeline data from Time-Series Database failed: {}", res.get_error()));
    }

    if (res.get_history().is_raw())
    {
        throw std::runtime_error(
            "Requested an aggregate, got raw values from Time-Series DB instead!");
    }
    *aggr = res.get_history().get_aggr()[0];
    conn.shutdown();
}

msg::Aggregate get_aggregate(client::Connection& conn, Address addr, uint64_t start, uint64_t end)
{
    auto db_addr = resolve(conn, { { "service", "time_series_db" } });

    if (db_addr.size() != 1)
    {
        throw std::runtime_error(
            "Could not resolve address of time-series DB service, is it running?");
    }

    msg::Aggregate aggr;

    conn.request(db_addr[0],
                 msg::HistoryReqV1(addr, start, end, 0, msg::HistoryType::AGGREGATE).to_capnp(),
                 get_aggregate_res, &aggr);
    client::SimpleEventLoop sel(conn);

    sel.run();

    conn.reset_shutdown();

    return aggr;
}
} // namespace helpers
} // namespace seanerbus
