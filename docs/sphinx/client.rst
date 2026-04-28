Client Libraries
================

This is the documentation on the client libraries for C++/Python/Rust, that can be used to publish, subscribe, request and respond on the data-plane.

These data-plane communication primitives, together with the `standardized payloads <msg.html>`_  (e.g. ``TimeValueV1`` for sending ``(timestamp, metric-reading)`` pairs) can be used for efficient data-plane communication.

For examples on building data-plane clients, see the ``cpp_examples``/``rust_examples`` and ``python_examples`` folder in the `SeanerBUS repostiory <https://gitlab.seanergys.fz-juelich.de/software/seanerbus/>`_.


These libraries all implement the `SeanerBUS line-protocol <line_protocol.html>`_.

.. toctree::
   client_cpp
   client_python
   crates/rust_client/bindings/rust/lib
