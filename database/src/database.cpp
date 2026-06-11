// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <hta/chrono.hpp>
#include <hta/hta.hpp>
#include <hta/types.hpp>
#include <log.hpp>
#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/helpers/resolve.hpp>
#include <seanerbus/internal/message.hpp>
#include <seanerbus/msg/helpers.hpp>
#include <seanerbus/msg/history_req_v1.hpp>
#include <seanerbus/msg/history_res_v1.hpp>
#include <seanerbus/msg/time_value_v1.hpp>

#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "seanerbus/msg/msg.capnp.h"

// Converts a single hta::Aggregate to a Dataplane Aggregate
seanerbus::msg::Aggregate hta_aggr_to_sean_aggr(hta::Aggregate aggr)
{

    seanerbus::msg::Aggregate res;

    res.minimum = aggr.minimum;
    res.maximum = aggr.maximum;
    res.sum = aggr.sum;
    res.count = aggr.count;
    res.integral = aggr.integral;
    res.activeTime = aggr.active_time.count();

    return res;
}

// Converts a vector of hta::Row's into a vector of timestamps and Aggregates as used by the
// Dataplane
std::pair<std::vector<int64_t>, std::vector<seanerbus::msg::Aggregate>>
hta_rows_to_sean_aggr(std::vector<hta::Row> aggrs)
{

    auto res = std::make_pair(std::vector<int64_t>{}, std::vector<seanerbus::msg::Aggregate>{});
    for (const auto& aggr : aggrs)
    {
        res.first.emplace_back(aggr.time.time_since_epoch().count());
        res.second.emplace_back(hta_aggr_to_sean_aggr(aggr.aggregate));
    }

    return res;
}

// Converts a vector of hta::TimeValues to timestamps and doubles as used by the Dataplane History
std::pair<std::vector<int64_t>, std::vector<double>>
hta_time_values_to_sean_raws(std::vector<hta::TimeValue> rows)
{
    auto res = std::make_pair(std::vector<int64_t>{}, std::vector<double>{});

    for (const auto& row : rows)
    {
        res.first.emplace_back(row.time.time_since_epoch().count());
        res.second.emplace_back(row.value);
    }

    return res;
}

void sub_handler(seanerbus::client::Connection* conn, seanerbus::Message msg, hta::Directory* dir)
{
    if (msg.payload.first == (uint64_t)::MessageTypes::TIME_VALUE_V1)
    {
        // Big ole try-catch(everything) block, because we do not want the DB to crash like ever.
        // @bielert1 assured me that exceptions only signal recoverable errors.
        //
        // In case the consistency of the HTA is going awry, the HTA will explode far more violently
        try
        {
            seanerbus::msg::TimeValueV1 tv(msg.payload);

            seanerbus::logging::debug("Inserting {}: {} for address {}", tv.time, tv.value,
                                      msg.address.as_string());
            auto& metric = (*dir)[std::string(msg.address.as_string())];

            metric.insert(
                hta::TimeValue{ hta::TimePoint(hta::Duration(tv.time)), (double)tv.value });
            metric.flush();
        }
        catch (std::exception& e)
        {
            seanerbus::logging::warn("Inserting time-value pair failed: {}, ignoring!", e.what());
        }
    }
    else
    {
        seanerbus::logging::warn(
            "Expected a TimeValueV1 from subscription to {}, got {} instead. Ignoring!",
            (uint64_t)MessageTypes::TIME_VALUE_V1, msg.address.as_string(),
            seanerbus::msg::payload_type_to_str(msg.payload.first));
    }
}

// Handler for HistoryReqV1
void history_req_handler(seanerbus::client::Connection* conn, seanerbus::Message msg,
                         hta::Directory* dir)
{
    if (msg.payload.first == (uint64_t)::MessageTypes::HISTORY_REQ_V1)
    {
        try
        {
            auto req = seanerbus::msg::HistoryReqV1(msg.payload);

            auto start = hta::TimePoint(hta::duration_cast(std::chrono::nanoseconds(req.start)));
            auto end = hta::TimePoint(hta::duration_cast(std::chrono::nanoseconds(req.end)));

            if (req.type == seanerbus::msg::HistoryType::AGGREGATE)
            {
                seanerbus::logging::debug("Data from {} ({}-{}) requested (AGGREGATED)",
                                          req.addr.as_string(), start.time_since_epoch().count(),
                                          end.time_since_epoch().count());

                auto& metric = (*dir)[req.addr.u_.as_string()];

                auto aggr = hta_aggr_to_sean_aggr(metric.aggregate(start, end));

                seanerbus::logging::debug(
                    "Got aggregate min:{}, max:{}, sum={}, count={}, integral={}, active_time={}",
                    aggr.minimum, aggr.maximum, aggr.sum, aggr.count, aggr.integral,
                    aggr.activeTime);

                conn->respond_to(msg,
                                 seanerbus::msg::HistoryResV1::make_aggr({}, { aggr }).to_capnp());
            }
            else
            {
                seanerbus::logging::debug("Data from {} ({}-{}) requested (TIMELINE)",
                                          req.addr.as_string(), start.time_since_epoch().count(),
                                          end.time_since_epoch().count());

                auto& metric = (*dir)[req.addr.as_string()];

                auto res = metric.retrieve_flex(
                    start, end, hta::duration_cast(std::chrono::nanoseconds(req.interval_max)));

                // Check if we are getting raw (hta::TimeValue in HTA speak) or aggregated
                // (hta::Row) values
                if (std::holds_alternative<std::vector<hta::TimeValue>>(res))
                {
                    auto raw_data =
                        hta_time_values_to_sean_raws(std::get<std::vector<hta::TimeValue>>(res));

                    seanerbus::logging::debug("Got raw timeline result: {} items",
                                              raw_data.first.size());

                    conn->respond_to(
                        msg, seanerbus::msg::HistoryResV1::make_raw(raw_data.first, raw_data.second)
                                 .to_capnp());
                }
                else
                {
                    auto aggr_data = hta_rows_to_sean_aggr(std::get<std::vector<hta::Row>>(res));

                    seanerbus::logging::debug("Got aggregated timeline result: {} items",
                                              aggr_data.first.size());

                    conn->respond_to(msg, seanerbus::msg::HistoryResV1::make_aggr(aggr_data.first,
                                                                                  aggr_data.second)
                                              .to_capnp());
                }
            }
        }
        catch (std::exception& e)
        {
            seanerbus::logging::warn("Servicing HistoryReqV1 failed: {}", e.what());

            conn->respond_to(
                msg, seanerbus::msg::HistoryResV1::make_error(
                         fmt::format("Got exception from HTA Time-Series Database: {}", e.what()))
                         .to_capnp());
        }
    }
    else
    {
        seanerbus::logging::warn(
            "Client {}: expected HistoryResV1, got {} instead. Answering with error",
            msg.from->u_.as_string(), seanerbus::msg::payload_type_to_str(msg.payload.first));

        conn->respond_to(
            msg, seanerbus::msg::HistoryResV1::make_error("Not a HistoryReqV1!").to_capnp());
    }
}

int main(void)
{
    seanerbus::logging::set_log_level(seanerbus::logging::LogLevel::TRACE);

    hta::json config{ { "type", "file" },
                      { "path", "./data" },
                      { "metrics", hta::json::object() } };

    std::vector<seanerbus::Address> metrics;
    std::ifstream metrics_file("metrics.list");

    std::string line;

    while (std::getline(metrics_file, line))
    {
        metrics.emplace_back(seanerbus::Address(line));
    }

    if (metrics.empty())
    {
        std::cerr << "Need to define at least one metric to record\n";
        std::cerr << "To record a metric, add a line with the UUID-Address of the metric to the "
                     "file metrics.list!\n";
        return 1;
    }

    for (auto metric : metrics)
    {
        config["metrics"][metric.as_string()] = hta::json{
            { "mode", "RW" },
            { "interval_min", 40000000000 },
            { "interval_max", 400000000000000 },
            { "interval_factor", 10 },
        };
    }

    hta::Directory dir(config, true);
    seanerbus::client::Connection conn("localhost", "5398");

    for (auto metric : metrics)
    {
        seanerbus::logging::debug("subscribing to {}", metric.as_string());
        conn.subscribe(metric, sub_handler, &dir);
    }

    auto history_req_addr = seanerbus::Address();

    seanerbus::logging::info("Using address {} for HistoryReqV1 interface",
                             history_req_addr.as_string());

    // Register address for HistoryReqV1 req/res
    conn.register_endpoint(history_req_addr, history_req_handler, &dir);

    // Register us a {"service" : "time_series_db"} to the naming serve, so that we can be found
    seanerbus::helpers::register_resolve(conn, { { "service", "time_series_db" } },
                                         history_req_addr);

    seanerbus::logging::debug("Entering Main Event Loop!");
    seanerbus::client::SimpleEventLoop sel(conn);

    sel.run();
    seanerbus::logging::info("Database received SIGINT, shutting down...");

    conn.reset_shutdown();
    seanerbus::helpers::unregister_resolve(conn, history_req_addr);
}
