.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Server Component
================

.. toctree::
   server_architecture
   line_protocol

The SeanerBUS prototype uses a single central server component for handling publish, subscribe, request and response communication.

This server component is engineered for easy extendability (for testing out Data-Plane) concepts, correctness, but not speed.

Usage
-----

The Server component is written in Rust.

Use `rustup <rustup.rs>`_ to install Rust or check with your distribution on how to install the required Rust tooling.

.. code-block:: sh

    cd server
    # This builds and runs the server
    cargo run
    # This does the same while given much more debug output
    RUST_LOG=trace cargo run

The server can be stopped with Ctrl-C.
