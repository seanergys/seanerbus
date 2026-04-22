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
- VectorReqV1 A request type containing a vector of float values.
- VectorResV1 A response type containing a vector of float values.

.. doxygenfunction:: seanerbus::msg::payload_type_to_str

.. doxygenclass:: seanerbus::msg::TimeValueV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::HelloWorldReqV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::HelloWorldResV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::VectorReqV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::VectorResV1

.. doxygenclass:: seanerbus::msg::ResolveReqV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::ResolveResV1
   :project: cpp_seanerbus
   :members:

.. doxygenclass:: seanerbus::msg::HistoryReqV1
   :members:

.. doxygenclass:: seanerbus::msg::HistoryResV1
   :members:
