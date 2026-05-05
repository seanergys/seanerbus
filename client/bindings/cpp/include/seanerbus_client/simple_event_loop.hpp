// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <seanerbus_client/connection.hpp>

#include <stdexcept>

extern "C"
{
#include <poll.h>
}

/**
 * A very simple poll() based event-loop, holding one Connection.
 *
 */
namespace seanerbus
{
namespace client
{
class SimpleEventLoop
{
public:
    /**
     * Creates a new SimpleEventLoop that handles the given connection
     */
    SimpleEventLoop(Connection& connection) : connection_(connection)
    {
    }

    /**
     * Runs the event-loop, effectively poll()-ing on the underlying fd and notifying try_read()
     * when it is appropriate.
     */
    void run()
    {
        struct pollfd pfds[1];

        pfds[0].fd = connection_.get_fd();
        pfds[0].events = POLLIN;
        pfds[0].revents = 0;

        while (true)
        {
            poll(pfds, 1, -1);

            if (pfds[0].revents == POLLHUP)
            {
                return;
            }

            if (pfds[0].revents == POLLIN)
            {
                connection_.try_read();

                if (connection_.is_shutdown())
                {
                    return;
                }
            }
            else
            {
                throw std::runtime_error("Unknown poll revent!");
            }
        }
    }

private:
    Connection& connection_;
};
} // namespace client
} // namespace seanerbus
