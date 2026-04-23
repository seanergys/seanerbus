// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus_client/connection.hpp>
#include <seanerbus_msg/hello_world_req_v1.hpp>
#include <seanerbus_msg/hello_world_res_v1.hpp>

#include <iostream>
#include <stdexcept>

#include <schemas/msg.capnp.h>
#include <unistd.h>

void res_handler(seanerbus::client::Connection& conn, seanerbus::Message msg, void* userdata)
{
    if (msg.type != ::Message::MessageType::REQUEST)
    {
        throw std::runtime_error("Got something else than a request!");
    }

    if (msg.payload.first != (uint64_t)::MessageTypes::HELLO_WORLD_REQ)
    {
        throw std::runtime_error("Got something else than a HelloWorldReqV1!");
    }

    seanerbus::msg::HelloWorldReqV1 req(msg.payload);

    std::cout << "Got incoming HelloWorldReqV1: " << req.str_msg << std::endl;

    seanerbus::msg::HelloWorldResV1 res(std::string("Thank you for your \"") + req.str_msg +
                                        std::string("\""));

    conn.respond_to(msg, res.to_capnp());
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

    conn.register_endpoint(addr, res_handler, nullptr);

    conn.main_loop();

    return 0;
}
