.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Standardized Payloads
=====================

The actual communication over the dataplane is facilitated through the use of standardized payloads to the publish/subscribe/request and response communication primitives.

The currently defined payloads and their payloads types are documented in the section `on standardized payloads <msg_capnp.html>`_  as their canonicial Cap'n'Proto format.

The `New Payload Types HOWTO <new_msg_type.html>`_ shows how you can define a new payload type.

Additionally, There a convenience Rust/C++/Python payload encoding/decoding libraries.

.. toctree::
   msg_capnp
   new_msg_type
   msg_cpp
   msg_python
   crates/rust_msg/bindings/rust/lib
