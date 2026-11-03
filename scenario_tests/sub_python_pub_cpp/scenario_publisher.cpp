// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/helpers/noexcept.hpp>
#include <seanerbus/helpers/time_series.hpp>

#include "seanerbus/time/time.hpp"

uint64_t timestamp = 0;
double value = 0.0;
seanerbus::msg::Entity entity;

void on_publish(seanerbus::client::Connection& conn,
                seanerbus::Expected<void, seanerbus::Error> err)
{
    if (timestamp >= 4)
    {
        conn.shutdown();
    }
    else
    {
        timestamp++;
        value++;
        seanerbus::time_series::publish_time_value(
            conn, entity, seanerbus::time::clock::time_point(std::chrono::nanoseconds(timestamp)),
            value, on_publish);
    }
}

int main(int argc, char** argv)
{
    entity = seanerbus::msg::Entity(seanerbus::msg::Uuid(0, 10));
    seanerbus::client::Connection conn("seanerbroker://localhost:5398", "seanerbroker://localhost:5398");

    on_publish(conn, seanerbus::Expected<void, seanerbus::Error>());
    seanerbus::client::SimpleEventLoop loop(conn);

    loop.run();
}
