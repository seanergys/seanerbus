.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Base Payloads
=============

For the communication over the data-plane, the SeanerBUS uses standardized Cap'n'Proto payloads, that together with the standardized `communication primitives <client.html>`_ and the `addressing and naming scheme <addressing.html>`_ provide a full description of the interactions with the data-plane.

.. note::
   This is the prototype implementation of the data-plane, so no actual standardization has happened yet.

Payload Types
-------------

Every standardized payload has a payload type.

When sending a payload, the payload type is sent with it to describe it.

The enumeration below defines the current defined payload types:

.. code-block:: python

    enum MessageTypes
    {
        timeValueV1 @0;
        namingServiceReqV1 @1;
        namingServiceResV1 @2;
        helloWorldReqV1 @3;
        helloWorldResV1 @4;
    }

Below, the payload types are defined:

Time-Value Pairs
----------------

Most metrics will appear as pairs of a timestamp and a recorded value.

This payload type encodes such a time-value pair.

.. code-block:: python

    struct TimeValueV1
    {
        time @0 :UInt64;
        value @1 :Float64;
    }

Naming Service Requests and Responses
-------------------------------------

A pair of request and response encodings for use with the naming-service.
The details of the naming service are described `in its own section <addressing.html>`_.

A client which wants to resolve a name (e.g. ``{node=4, metric=power}``) will encode this as a NamingServiceRequestV1 by putting the key value pairs in ``attributes``.

.. code-block:: python

   NamingServiceRequestV1.attributes=[{key="node", value="4"}, {key="metric", value="power"}]

The naming service will respond to a NamingServiceRequestV1 with a NamingServiceResponseV1, which contains in ``matchingInterfaces`` a list of all addresses which match the given key-value pairs.

.. code-block:: python

    struct NamingServiceReqV1
    {
    struct KV
    {
    key @0 :Text;
    value @1 :Text;
    }
    attributes @0 :List(KV);
    }

    struct Address{
    lower @0 :UInt64;
    upper @1 :UInt64;
    }

    struct NamingServiceResV1
    {
    matchingInterfaces @0 :List(Address);
    }


Hello World Example Requests and Responses
------------------------------------------

A pair of simple request and responses used by the request_test and response_test
examples in `(cpp|rust|python)_examples`.

In the example implementation, the requester requests a HelloWorldReqV1 with a simple string in it.
In turn, the responder responds with a HelloWorldResV1 that echoes the string in the HelloWorldReqV1 back to it.

.. code-block:: python

    struct HelloWorldReqV1
    {
    msg @0 :Text;
    }

    struct HelloWorldResV1
    {
    msg @0 :Text;
    }
