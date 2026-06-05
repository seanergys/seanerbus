.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Defining a new payload type HOWTO
=================================

Payloads are defined in `msg/schemas/msg.capnp <https://gitlab.seanergys.fz-juelich.de/software/seanerbus/-/blob/aa09a489ed810038a6c87b0f077e91143ed742a2/msg/schemas/msg.capnp>`_. The `Cap'n'Proto Schema Language is defined here <https://capnproto.org/language.html>`_.

.. note::
   For complicated Python packaging reasons, there is a duplicate version of the `msg.capnp` in `bindings/python/src/seanerbus/msg/msg.capnp`. The CI will warn you though if the `msg.capnp` differ, though.

A payload consists of two parts:

1. A unique payload type.
2. A Cap'n'Proto schema definition of the payload.

Payload Types
-------------

When sending a payload with the SeanerBUS, a pair of a payload type (an int) and the bytes of the payload is sent.

By checking the payload type, receivers can check what was sent to them and then decode the payload bytes accordingly. 

.. note::
   In the end, the payload type is the whole contract between sender and receiver. So as long as the client knows what to do with the blob of payload data given some payload type. **Any** encoding is fine. However it is **not** advised to use something else than Cap'n'Proto.

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
   Cap'n'Proto enforces CamelCase on types, so myCoolTypeV1, not mycooltypev1 or my_cool_type_v1

In the future, these payloads should be standardized and versioned. The added ``V1`` makes it easy to later add a ``myCoolTypeV2`` that fixes a bunch of issues we introduced in the first version.

With the payload type defined, you can already check for it in, for example, Python code:

.. code-block:: python

   from seanerbus.msg import msg_capnp
   from seanerbus.client import Connection

   c = Connection.connect("localhost", 5398)

   [...]

   msg = c.read_msg

   if msg.payload_type == msg_capnp.MessageTypes.myCoolTypeV1:
       print("Got My Cool Type V1 payload!")

   [...]

   c.publish(some_addr, (msg_capnp.MessageTypes.myCoolTypeV1, some_my_cool_type_v1_bytes))

Payload Encoding
----------------

Secondly we want to safely and unambiguously encode and decode the data associated with the payload type.
For this, we have in this project opted for Cap'n'Proto.

To give an examples of such a payload encoding, `msg/schemas/msg.capnp <https://gitlab.seanergys.fz-juelich.de/software/seanerbus/-/blob/aa09a489ed810038a6c87b0f077e91143ed742a2/msg/schemas/msg.capnp>`_, for the ``timeValueV1`` payload type the ``TimeValueV1`` payload encoding is defined as:

.. code-block::

    struct TimeValueV1
    {
        time @0 :UInt64;
        value @1 :Float64;
    }

(Again, the `Cap'n'Proto Schema Language is defined here <https://capnproto.org/language.html>`_.)

If we want to send two 64-bit unsigned integers with `MyCoolTypeV1`, we would define:

.. code-block::

   struct MyCoolTypeV1
   {
       first_field @0 :UInt64;
       second_field @1 :UInt64;
   }

If we put this into ``msg/schemas/msg.capnp``, code for handling MyCoolTypeV1 will be automatically generated. To use it from Python, for example:


.. code-block:: python

   from seanerbus.msg import msg_capnp
   from seanerbus.client import Connection

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

The payload encoding, in the end, only defines what bits go into the message, not necessarily what any of those bits mean.

If for some reason ``MyCoolTypeV1`` requires that ``second_field`` must always be numerically larger than ``first_field``, then this can not be expressed in Cap'n'Proto directly.

Likewise, for more complicated types (dictionaries, multi-level object hierarchies, etc.). Repeating the same code to encode those types in Cap'n'Proto can become tedious fast.

For all current SeanerBUS payloads, we have thus additionally defined convenience payload wrapper classes (`for Python, for example <msg_python.html>`_) that hide away all the ugly details
of the Cap'n'Proto underneath.

Even further, most users of the dataplane probably have no need for interacting with the payloads directly. For more complex use-cases, like time-series database lookups, we have thus built
full harness function which hide away the whole interaction.

Standardization
---------------

To be discussed beyond the prototype/demonstrator stage.
