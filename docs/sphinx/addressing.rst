Addressing and Naming
=====================

.. warning::
   This reflects the Addressing and Naming Scheme presented by TUD in the WP2 working group and is a clear **work-in-progress**.

The current prototype implementation of the data-plane makes use of a two level naming scheme:

1. lower level **Address** es, which are `UUIDv4 <https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)>`_ identifiers. (e.g. ``7dcbbd15-e16d-45e0-a767-000a583345d0``)
2. higher-level **Name** s, which are sets of key-value pairs. (e.g. ``{rack=4, node=5, metric=power}``)

This two-level scheme can be seen akin to the role IPs and Domain Names play in network communication.

The data-plane itself only uses the addresses, e.g. when you want to request some service, you give a UUIDv4:

.. code-block:: python

    await c.request(
            uuid.UUID("7d47dea4-ddb3-4134-8abf-b9d78113132d"),
            HelloWorldReqV1("Hello World").to_capnp())

The names make it easier to speak about certain sets of interfaces: "the power sensor of all nodes in rack 4" is equivalent to: ``{rack=4, node=*, metric=power}``.

Naming Service interface
------------------------

The naming service is a request/response interface build on the `Naming Service Request and Response payload definitions <msg_capnp.html#naming-service-requests-and-responses>`_ specified there.

To avoid boot-strapping problems, the naming service is the only part of the data-plane with a non-randomly-generated  address ('00000000-0000-0000-0000-0000010ca105', "locator" in leetspeak).

.. note::
   If its anywhere possible, the Naming Service MUST also remain the only service with a non-random Address. Random generation of addresses is what makes it possible to create unique addresses 

On '00000000-0000-0000-0000-0000010ca105' it offers a request/response interface ingesting ``NamingServiceRequestV1``'s and return ``NamingServiceResponseV1``.

A detailed description of the interface is given in the definition of the `Naming Service Request/Response Payloads  <msg_capnp.html#naming-service-requests-and-responses>`_.

In Short, a client sends a ``NamingServiceRequestV1`` into which it puts the ``key=value`` pairs of the name(s) it wants to look up.
The Naming Server responds with a ``NamingServiceResponseV1`` which contains 0..n UUID addresses of the names in its database which matched the ``NamingServiceRequestV1`` ``key=value`` pairs.

Be mindful that any such query can return more than one address.

If the query were ``{rack=4, node=*, metric=power}``, then the naming server would return the address of as many power sensors in nodes of rack 4 as there are nodes in rack 4.

Naming Server Prototype
-----------------------

``naming_scheme/`` contains a prototype for a naming service.


To run the naming sevice itself.

.. code-block:: sh

   cd naming_scheme
   cargo run --bin naming_server

The prototype naming server uses a simple ``services.json`` file as a database.

The file below gives an indication of the ``services.json`` format:


.. code-block:: json

    [
        {
            "uuid": "57499619-8305-44b8-ae52-c75b93d20ff9",
            "attrs": {"node": "4", "sensor": "freq"}
        },
        {
            "uuid": "c70de76e-2157-43c6-b31d-d9aabebe166a",
            "attrs": {"node": "5", "sensor": "freq"}
        },
        {
            "uuid": "09011c23-439b-43f3-bfc5-5558c130d094",
            "attrs": {"node": "6", "sensor": "power"}
        }
    ]
