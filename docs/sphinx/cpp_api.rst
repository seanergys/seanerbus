.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

C++ API
=======

The C++ API contains these parts:

- The `C++ client library <client_cpp.html>`_ for connecting to and doing pub/sub and req/res on the dataplane.
  - The C++ client library is non-blocking and callback-based and thus requires an event-loop to poll on the connection. For simple applications a ``SimpleEventLoop`` is included.
- The `C++ payload library <msg_cpp.html>`_ for serializing and deserializing SeanerBUS payloads.
- The `C++ naming library <naming_cpp.html>`_ for interacting with the naming service. (e.g. to resolve a name to an address).
- The `C++ time-series library <time_series_cpp.html>`_ for querying historic time-series data.


.. toctree::
   client_cpp
   msg_cpp
   naming_cpp
   time_series_cpp
