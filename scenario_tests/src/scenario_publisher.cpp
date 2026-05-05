// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus_client/connection.hpp>
#include <seanerbus_msg/time_value_v1.hpp>

#include <iostream>
#include <stdexcept>

#include <schemas/msg.capnp.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    seanerbus::client::Connection conn("localhost", "5398");

    uint64_t time = 0;
    double value = 0.0;

    auto addr = seanerbus::Address{ seanerbus::Uuid{ 0, 10 } };
    for (int i = 0; i < 3; i++)
    {
        seanerbus::msg::TimeValueV1 tv{ time, value };

        conn.publish(addr, tv.to_capnp());

        time++;
        value++;
    }
}
