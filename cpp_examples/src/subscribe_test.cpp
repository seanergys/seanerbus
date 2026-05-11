// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/msg/time_value_v1.hpp>

#include <iostream>
#include <stdexcept>

void sub_handler(seanerbus::client::Connection& conn, seanerbus::Message message)
{
    // Check the payload type if we are really dealing with a TimeValueV1 message.
    //
    // TODO: The name of this enum could be nicer.
    if (message.payload.first == (uint64_t)::MessageTypes::TIME_VALUE_V1)
    {
        seanerbus::msg::TimeValueV1 v(message.payload);

        std::cout << "time=" << v.time << ", value=" << v.value << std::endl;
    }
    else
    {
        throw std::runtime_error("Got unexpected message type!");
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "./publish_test UUID" << std::endl;
        return -1;
    }

    // Connect to the server.
    seanerbus::client::Connection conn("localhost", "5398");

    // Subscribe to the Address given on the command line.
    conn.subscribe(seanerbus::Address{ seanerbus::Uuid{ argv[1] } }, sub_handler);

    // Run the main loop, reading for messages.
    seanerbus::client::SimpleEventLoop sel(conn);
    sel.run();
}
