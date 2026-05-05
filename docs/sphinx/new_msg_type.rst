.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Defining a new payload type HOWTO
=================================

The definition of all payloads is currently contained in the `msg/schemas/msg.capnp <https://gitlab.seanergys.fz-juelich.de/software/seanerbus/-/blob/aa09a489ed810038a6c87b0f077e91143ed742a2/msg/schemas/msg.capnp>`_ Cap'n'Proto schema definition file. The `Cap'n'Proto Schema Language is defined here <https://capnproto.org/language.html>`_.

As already discussed in the listing of the currently standardized payloads, a payload consists of two parts:

1. A unique payload type.
2. A Cap'n'Proto schema definition of the underlying message.

Payload Types
-------------

The payload type **types** the payload.
Clients will use this to differentiate between different payload types.

.. note::
   In the end, the payload type is the whole contract between sender and receiver. So as long as the client knows what to do with the blob of payload data given a payload type. **Any** encoding is fine. However it is **not** advised to use something else than Cap'n'Proto.

The payload types are listed in the enumeration ``MessageTypes``.

.. code-block::

    enum MessageTypes
    {
        timeValueV1 @0;
        namingServiceReqV1 @1;
        namingServiceResV1 @2;
        helloWorldReqV1 @3;
        helloWorldResV1 @4;
    }

To define a new type, add a new value to this enum:

.. code-block::

    enum MessageTypes
    {
        [...]
        helloWorldResV1 @4;
        myCoolTypeV1 @5;
    }


.. note::
   Cap'n'Proto enforces CamelCase on Types, so myCoolTypeV1, not mycooltypev1 or my_cool_type_v1

With regards to the standardization we are pursuing, the added ``V1`` makes it easy to later add a ``myCoolTypeV2`` that fixes a bunch of issues we introduced in the first version.

In python you can now check for the new payload type:

.. code-block:: python

   from seanerbus_msg import msg_capnp
   from seanerbus_client import Connection

   c = Connection.connect("localhost", 5398)

   [...]

   msg = c.read_msg

   if msg.payload_type == msg_capnp.MessageTypes.myCoolTypeV1:
       print("Got My Cool Type V1 payload!")

   [...]

   c.publish(some_addr, (msg_capnp.MessageTypes.myCoolTypeV1, some_my_cool_type_v1_bytes))

Payload Encoding
----------------

Secondly we want to safely and umabiguously encode and decode the data associated with the payload type.
For this, we have in this project opted for Cap'n'Proto.

In `msg/schemas/msg.capnp <https://gitlab.seanergys.fz-juelich.de/software/seanerbus/-/blob/aa09a489ed810038a6c87b0f077e91143ed742a2/msg/schemas/msg.capnp>`_, the associated payload encoding for the ``timeValueV1`` encoding is defined as:

.. code-block::

    struct TimeValueV1
    {
        time @0 :UInt64;
        value @1 :Float64;
    }

(Again, the `Cap'n'Proto Schema Language is defined here <https://capnproto.org/language.html>`_.)

If we want to transport two 64-bit unsigned integers, we would define:

.. code-block::

   struct MyCoolTypeV1
   {
       first_field @0 :UInt64;
       second_field @1 :UInt64;
   }

If we put this into ``msg/schemas/msg.capnp``, the correct code for creating MyCoolTypeV1 payloads is automatically generated, in Python this looks like this:


.. code-block:: python

   from seanerbus_msg import msg_capnp
   from seanerbus_client import Connection

   c = Connection.connect("localhost", 5398)

   [...]

   # On the sender end:

   my_cool_msg = msg_capnp.MyCoolTypeV1.new_message()
   my_cool_msg.first_field = 42;
   my_cool_msg.second_field = 0xdeadbeef


   await c.publish(some_addr, (msg_capnp.MessageTypes.myCoolTypeV1, my_cool_msg.to_bytes()))

   [...]

   # On the receiver end:

   msg = await c.read_msg()

   if msg.payloadType == msg_capnp.MessageTypes.myCoolTypeV1:
       print("Got My Cool Type V1 payload!")
       my_cool_msg = msg_capnp.MyCoolTypeV1.from_bytes(msg.payload)
       print(f"Got first_field={my_cool_msg.first_field}, second_field={my_cool_msg.second_field}")

This is already everything required to send a new payload safely over the SeanerBUS!.

Payload Encoding Libraries etc.
-------------------------------

To ease the use of the payload types, the SeanerBUS does not usually require its users to interact with the raw Cap'n'Proto defined classes.

Instead, we have build C++, Rust and Python libraries which ease working with the payload types.

Simply have a look at the already defined convenience wrappers in those libraries to get an idea how to define your own.

This is also important because the Cap'n'Proto can only carry encoding information, not semantic information.

If, for MyCoolTypeV1, first_field can never be 0, then there need to be other ways to enforce this.
This is easiest done with a convenience wrapper class for the type that enforces exactly that.

Standardization
---------------

To be discussed beyond the prototype/demonstrator stage.
