.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

.. SeanerBUS documentation master file, created by
   sphinx-quickstart on Tue Apr 28 13:20:43 2026.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

SeanerBUS documentation
=======================

The SeanerBUS is a prototype implementation of the SEANERGYS data-plane that supports publish, subscribe, request and response semantics.

The SeanerBUS consists of a `server <server.html>`_ component, which facilitates the communication between the clients.

It also consists of `client libraries <client.html>`_ for Python, Rust and C++.
These allow programmers to easily publish, subscribe, request and respond data.

The content of the communicated message is expressed through standardized, `Cap'n'Proto encoded payloads <msg.html>`_.
To publish e.g. a time-value pair (as for example from a sensor reading), encode it first as a ``TimeValueV1`` payload and then use one of client libraries ``publish`` functions to publish it to the SeanerBUS.

Lastly it makes use of a `two-level addressing and naming scheme <addressing.html>`_ in which expressive names consisting of ``tag=value`` pairs. (e.g. ``{ node = 3, sensor = power}``) are resolved through a naming service to efficient and guaranteed to be unique UUID addresses (e.g. ``052454b5-a27e-45d9-a9d8-d9288bb767e4``) that are actually used on the underlying transport.

.. toctree::
   :caption: Contents:

   server
   msg
   addressing
   cpp_api
   rust_api
   python_api
