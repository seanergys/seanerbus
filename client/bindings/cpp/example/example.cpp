// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus_client/connection.hpp>

#include <iostream>

void sub_handler(seanerbus::Message, void* userdata)
{
    std::cout << "Got value!" << std::endl;
}

int main()
{
    seanerbus::client::Connection conn("localhost", "5398");

    conn.subscribe(seanerbus::Address{ seanerbus::Uuid{ 0, 1 } }, sub_handler, nullptr);

    conn.main_loop();
}
