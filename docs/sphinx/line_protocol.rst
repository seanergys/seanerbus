.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

SeanerBUS Line Protocol
=======================

.. warning::
   This is the internal line-protocol that you use to communicate with the SeanerBUS itself.
   For information on how to create the message to send to other communication partners on the data-plane, see the `Standardized Payload <msg.html>`_ documentation.

   For your favorite language there is probably already a finished implementation of this line-protocol in the `Client libraries <client.html>`_

The basic transport medium of the SeanerBUS are TCP sockets.
Per-default, the server component will listen on ``localhost:5398``.

Before any other interaction, clients need to connect via TCP to the server component.
The life-time of the TCP connection governs the lifetime of subscriptions and registered ``req/res`` services.
When the connection is lost (because the socket is e.g. closed) all subscription are canceled and all registered ``req/res`` services disappear.
There is no "keep-alive".


Once the connection has been made, the client can interact with the server by sending it Cap'n'Proto encoded messages.

.. note::
   These basic line-protocol messages are **not** to be confused with the `Standardized Payloads <line-protocol.html>`_

The Cap'n'Proto definition of the line-protocal messages is shown below:


.. code-block::
    
    struct Message
    {
        to @0 :Uuid;
        from @1 :Uuid;
        address @2 :Uuid;
        struct Uuid
        {
        upper @0 :UInt64;
        lower @1 :UInt64;
        }
        type @3 :MessageType;
        enum MessageType
        {
            subscribe @0;
            publish @1;
            error @2;
            request @3;
            register @4;
            response @5;
        }
        payloadType @4  :UInt64;
        payload @5 :Data;
        matchingId @6 :UInt64;
    }

    struct ErrorPayload
    {
        errorKind @0 :ErrorKind;
        enum ErrorKind {
            noSuchInterface @0;
        }
        errorStr @1 :Text;
    }

The ``type`` field gives the type of message, which can be any of the ``MessageType`` enumeration:

- ``subscribe`` -- Subscribe for ``pub/sub`` messages.
- ``publish`` -- Publish a ``pub/sub`` message.
- ``error`` -- The server reports an error.
- ``request`` -- Request a ``req/res`` interface.
- ``register`` -- Register a ``req/res`` interface.
- ``response`` -- Send a response to ``req/res`` request.

In the following examples, if a field is **not** set, its content are ignored by the server and should be ignored by the client.

Subscribe
---------

To subscribe to a ``pub/sub`` service:

.. code-block:: python

   msg.type = subscribe
   msg.address = some_addr

This subscribes the client to ``publish`` events from ``some_addr``.

The server will then begin to forward ``publish`` messages matching ``some_addr`` to this client.

Publish
-------

To publish a ``pub/sub`` message:

.. code-block:: python

   msg.type = publish
   msg.address = to_publish_to
   msg.payloadType = my_payload_type
   msg.payload = my_payload

``to_publish_to`` is the address the event will be published to.

The actual content that is published is encoded in ``my_payload_type`` and ``my_payload``.

This is required to be a SeanerBUS payload as defined in the `Standardized Payload <msg.html>`_.

The server component does **no** checks on the address.
Multiple publishers can publish to the same address at the same time.
It is also **not** an error to publish to an address to which no one has subscribed.
The message will be silently discarded.

Register
--------

This registers a ``req/res`` service, telling the daemon that ``req/res`` requests for this address should be forwarded to this client:

.. code-block:: python

   msg.type = register
   msg.address = offered_service_address

A second ``register`` for the same address will overwrite the first ``register``, so ``req/res`` requests are now forwarded to the second client.

``req/res`` registration will stay active as long as the client is connected.

After the ``register`` the client will start receiving ``request`` s for the registered address.

Request
-------

To request a ``req/res`` service, the client sends a ``request`` message:

.. code-block:: python

   msg.type = request
   msg.address = requested_service_addr
   msg.payload = my_req_payload
   msg.payloadType = my_req_payload_type
   msg.matchingId = some_random_matching_id # (e.g. 0xf00ba5)

``my_req_payload`` is the actual content of the request as a `Standardized Payload <msg.html>`_.

If there is no ``req/res`` service registered, the server will reply to the client with an ``error`` message with an ``ErrorPayload`` of ``noSuchInterface``.

If there is a ``req/res`` service registered, the server will forward the request to the registered responder.

A well-behaved ``req/res`` service will **always** respond to a ``request`` with a ``response``.

The ``matchingId`` is used to match requests to responds.
A response with the same ``matchingId`` as a request is the answer to that request.

These only need to be unique per communication partner. (e.g. the combination of ``msg.from`` and ``msg.matchingId`` must be unique).

If two clients both have a ``matchingId`` of 42, then those are **different** requests.

The server implementation puts no limit on the number of in-flight requests for t he same address.

Response
--------

When a request is received, clients are required to respond, to do this:

.. code-block:: python

   # This is the request we received
   request_msg
   response_msg.type = response
   response_msg.payload = my_res_payload
   response_msg.payloadType = my_res_payload_type
   response_msg.to = request_msg.from
   response_msg.matchingId = request_msg.matchingId

``my_res_payload`` is the actual content of the response as a `Standardized Payload <msg.html>`_.

The definition of the request payload will specify what the correct response payload is.

The ``request_msg.from`` field is an opaque identifier of the originating client.
This is applied by the SeanerBUS server to every message it receives.
By setting the ``response_msg.to`` to the value of ``request_msg.from``, the response is routed back to the requestee.

The ``matchingId`` of the response must be the same as the ``matchingId`` of the request.

Error
-----

Error messages are **only** sent by the server.
Sending an ``error`` message as a client is a violation of the line protocol and will lead to the server disconnecting the client.

In request/response scenarios where the requested operation can fail, the responder should send an ``response`` Message with
a `payload <msg.html>`_.
For signaling errors in ``req/res`` scenarios, send a ``response`` message with a `Messages <msg.html>`_ payload that indicates error instead of an ``error`` payload.

In an ``error`` message, the ``payload`` is always an ``ErrorPayload`` as shown above.

The client can decode the ``payload`` as an ``ErrorPayload`` to get the ``errorKind`` and an ``errorStr``.

``errorPayload.errorKind`` is an enumeration value of the different error types:

- ``noSuchInterface`` -- The client send a request to an address for which no ``req/res`` service is registered.

``errorPayload.errorStr`` holds a human-readable error message, that can be printed to e.g. logs.
