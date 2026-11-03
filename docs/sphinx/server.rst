.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

DemoBUS
=======

.. toctree::
   server_architecture
   line_protocol

For testing purposes, there is a simple, centralized, one-binary, req/res and pub/sub backend: The DemoBUS.

The DemoBUS uses URLs of the form ``seanerbroker://localhost:5398``

Usage
-----

The DemoBUS is written in Rust.

Use `rustup <rustup.rs>`_ to install Rust or check with your distribution on how to install the required Rust tooling.

.. code-block:: sh

    cd server
    # This builds and runs the server
    cargo run
    # This does the same while given much more debug output
    RUST_LOG=trace cargo run
    # Optionally, you can give it an address, otherwise localhost:5398 is used
    cargo run '0.0.0.0:9000'

The server can be stopped with Ctrl-C.
