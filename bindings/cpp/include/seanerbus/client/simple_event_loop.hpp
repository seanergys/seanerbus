// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <seanerbus/client/connection.hpp>

#include <iostream>
#include <stdexcept>
#include <system_error>

#include <csignal>

#include <bits/types/sigset_t.h>

extern "C"
{
#include <poll.h>
#include <sys/signalfd.h>
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
        pfds[0].fd = connection_.get_fd();
        pfds[0].events = POLLIN;
        pfds[0].revents = 0;

        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);

        if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        {
            throw std::system_error(errno, std::system_category());
        }
        pfds[1].fd = signalfd(-1, &mask, 0);
        pfds[1].events = POLLIN;
        pfds[1].revents = 0;
    }

    /**
     * Handler that is called if SIGINT (i.e. Ctrl+C) is received.
     *
     * By default, shuts the connection down.
     *
     * Override this function to supply your own SIGINT behaviour.
     */
    virtual void sigint_handler()
    {
        connection_.shutdown();
    }

    /**
     * Handler that is called if poll() times out.
     *
     * By default, does nothing.
     *
     * Override this function to supply your own time-out behaviour.
     */
    virtual void timeout_handler()
    {
    }

    /**
     * Runs the event-loop, effectively poll()-ing on the underlying fd and notifying try_read()
     * when it is appropriate.
     */
    void run()
    {
        while (true)
        {
            if (connection_.is_shutdown())
            {
                return;
            }
            int ret = poll(pfds, 2, 2000);

            if (ret == 0)
            {
                timeout_handler();
                continue;
            }

            if (ret == -1)
            {
                throw std::system_error(errno, std::system_category());
            }

            if (pfds[0].revents & POLLIN)
            {
                connection_.try_read();
            }

            if (pfds[1].revents & POLLIN)
            {
                sigint_handler();
            }
        }
    }

private:
    struct pollfd pfds[3];
    Connection& connection_;
};
} // namespace client
} // namespace seanerbus
