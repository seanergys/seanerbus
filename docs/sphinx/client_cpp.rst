.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

C++ Client Library
==================

This library provides a ``Connection`` class for connecting to and interacting with the data-plane.

The ``Connection`` class requires an event loop to poll on its file-descriptor.

You can either integrate your own or use the shipped ``SimpleEventLoop`` class exists, which calls ``poll()`` on the ``Connection`` s file descriptor. (And serves as an example of how to integrate the ``Connection`` into your own event loop)

This is a `RAII <https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization>`_ class, so the connection is open as long as the Connection object lives.

This only defines the communication primitives (pub/sub/req/res), for encoding the actual payloads, see the `C++ Payload Library <msg_cpp.html>`_.

.. doxygenclass:: seanerbus::client::Connection
   :project: cpp_seanerbus
   :members:
.. doxygenclass:: seanerbus::client::SimpleEventLoop
   :project: cpp_seanerbus
   :members:
.. doxygenstruct:: seanerbus::Address
   :project: cpp_seanerbus
   :members:
