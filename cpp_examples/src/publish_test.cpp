// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus_client/connection.hpp>
#include <seanerbus_msg/time_value_v1.hpp>

#include <iostream>
#include <stdexcept>

#include <schemas/msg.capnp.h>
#include <unistd.h>

void sub_handler(seanerbus::Message message, void* userdata)
{
    // Check the payload type if we are really dealing with a TimeValueV1 message.
    //
    // TODO: The name of this enum could be nicer.
    if (message.payload.payload_type ==
        (uint64_t)capnp::schemas::MessageTypes_b06630b2cb0c9ce8::TIME_VALUE_V1)
    {
        seanerbus::msg::TimeValueV1 v(message.payload.payload.data(),
                                      message.payload.payload.size());

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

    uint64_t time = 0;
    double value = 0.0;

    auto addr = seanerbus::Address{ seanerbus::Uuid{ argv[1] } };
    while (true)
    {
        seanerbus::msg::TimeValueV1 tv{ time, value };

        std::cout << "Publishing time=" << time << ", value=" << value << std::endl;
        auto res = tv.to_capnp();
        conn.publish(addr, res.first, res.second);

        sleep(1);
        time++;
        value++;
    }
    // Run the main loop, reading for messages.
    conn.main_loop();
}
