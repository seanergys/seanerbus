.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Addressing and Naming
=====================

.. warning::
   This reflects the Addressing and Naming Scheme presented by TUD in the WP2 working group and is a clear **work-in-progress**.

The current prototype implementation of the data-plane makes use of a two level naming scheme:

1. lower level **Address** es, which are `UUIDv4 <https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)>`_. (e.g. ``7dcbbd15-e16d-45e0-a767-000a583345d0``)
2. higher-level **Name** s, which are sets of key-value pairs. (In JSON notation ``{"rack": "4", "node": "5", "metric" : "power"}``)

This two-level scheme works similar to how IP addresses and Domain names split the work in the web.

The dataplane itself uses only the addresses. When you request an interface, you give it the address:

.. code-block:: python

    await c.request(
            address.UUID("7d47dea4-ddb3-4134-8abf-b9d78113132d"),
            HelloWorldReqV1("Hello World").to_capnp())

To use the Names, you have to translate them into addresses using the naming service.

Naming Service interface
------------------------

.. note::
   There are ready-to-use naming service libraries for `Python <naming_python.html>`_, `C++ <naming_cpp.html>`_ and Rust.

The naming service is a request/response interface build on the `Resolve Request and Response payload definitions <msg_capnp.html#naming-service-requests-and-responses>`_ specified there.

The naming service has a fixed address ``00000000-0000-0000-0000-0000010ca105``.

At ``00000000-0000-0000-0000-0000010ca105``, these req/res interfaces are offered:

- ``Resolve(Req/Res)V1`` name to addresses translation.
  - Importantly, this can give multiple addresses as a return value. If ``{"node" : "*", "metric" : "power"}``, then all node power sensors are returned.
- ``ReverseResolve(Req/Res)V1`` Address back to Name translation.
- ``RegisterResolve(Req/Res)V1`` Register new name -> address mappings.
- ``UnregisterResolve(Req/Res)V1`` Delete a address -> name mapping.

Naming Server Prototype
-----------------------

``naming_scheme/`` contains a prototype for a naming service written in Rust:

.. code-block:: sh

   cd naming_scheme
   cargo run --bin naming_server

The prototype naming server uses a simple ``services.json`` file as a database:

.. code-block:: json

    [
        {
            "address": "57499619-8305-44b8-ae52-c75b93d20ff9",
            "name": {"node": "4", "sensor": "freq"}
        },
        {
            "address": "c70de76e-2157-43c6-b31d-d9aabebe166a",
            "name": {"node": "5", "sensor": "freq"}
        },
        {
            "address": "09011c23-439b-43f3-bfc5-5558c130d094",
            "name": {"node": "6", "sensor": "power"}
        }
    ]
