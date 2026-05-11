.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

C++ Payload Library
===================

This library provides helper classes for encoding and decoding the standardized SeanerBUS payload types.:

This includes:

- TimeValueV1 A timestamp-value pair
- HelloWorldReqV1 A "Hello World" request type, consisting of only a string message.
- HelloWorldResV1 A "Hello World" response type, consisting of only a string message.

.. doxygenclass:: seanerbus::msg::TimeValueV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::HelloWorldReqV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::HelloWorldResV1
   :project: cpp_seanerbus
   :members:
