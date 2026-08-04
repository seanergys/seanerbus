// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/helpers/time_series.hpp>
#include <seanerbus/internal/message.hpp>

uint64_t timestamp = 0;
double value = 0.0;
seanerbus::Address addr;

void on_publish(seanerbus::client::Connection& conn, seanerbus::client::ConnectionError err)
{
    if (timestamp >= 4)
    {
        conn.shutdown();
    }
    else
    {
        timestamp++;
        value++;
        seanerbus::helpers::publish_time_value(conn, addr, timestamp, value, on_publish);
    }
}

int main(int argc, char** argv)
{
    addr = seanerbus::Address(seanerbus::Uuid(0, 10));
    seanerbus::client::Connection conn("localhost", "5398");

    on_publish(conn, seanerbus::client::ConnectionError::make_ok());
    seanerbus::client::SimpleEventLoop loop(conn);

    loop.run();
}
