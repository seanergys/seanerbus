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
    if (argc != 2)
    {
        std::cerr << "./publish_test UUID" << std::endl;
        return -1;
    }

    // Connect to the server.
    seanerbus::client::Connection conn("localhost", "5398");

    uint64_t time = 0;
    double value = 0.0;

    auto addr = seanerbus::Address{ seanerbus::Uuid{ argv[1] } };
    while (true)
    {
        seanerbus::msg::TimeValueV1 tv{ time, value };

        std::cout << "Publishing time=" << time << ", value=" << value << std::endl;
        conn.publish(addr, tv.to_capnp());

        sleep(1);
        time++;
        value++;
    }
    // Run the main loop, reading for messages.
    conn.main_loop();
}
