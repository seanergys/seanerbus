// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus_client/connection.hpp>
#include <seanerbus_client/simple_event_loop.hpp>
#include <seanerbus_msg/hello_world_req_v1.hpp>

#include <iostream>

#include <schemas/msg.capnp.h>
#include <unistd.h>

void req_handler(seanerbus::client::Connection& conn, seanerbus::Message msg)
{
    if (msg.payload.first != (uint64_t)::MessageTypes::HELLO_WORLD_RES_V1)
    {
        std::cerr << "Got unknown return message type!" << std::endl;
        return;
    }
    else
    {

        seanerbus::msg::HelloWorldReqV1 result(msg.payload);

        std::cout << "Got answer to request: " << result.str_msg << std::endl;
    }

    conn.shutdown();
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "./publish_test UUID" << std::endl;
        return -1;
    }
    auto addr = seanerbus::Address{ seanerbus::Uuid{ argv[1] } };

    // Connect to the server.
    seanerbus::client::Connection conn("localhost", "5398");

    auto res = seanerbus::msg::HelloWorldReqV1("Hello World").to_capnp();

    conn.request(addr, res, req_handler);

    seanerbus::client::SimpleEventLoop sel(conn);

    sel.run();

    return 0;
}
