.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Defined Payloads
================

This section lists all defined payloads in their definitive, Cap'n'Proto encoding.
For working with payloads in `Python <msg_python.html>`_, `C++ <msg_cpp.html>`_ or `Rust <crates/rust_msg/bindings/rust/message.html#seanerbus_msg-message-Message>`_ there already exist
Payload encoding and decoding libraries that hide away the gory Cap'n'Proto details.

.. note::
   This is the prototype implementation of the data-plane, so no actual standardization has happened yet.

User-Defined Payloads
---------------------

To send arbitrary data over the SeanerBUS, a user-defined payload can be used.
A user-defined payload is defined by a payload type larger than ``1 << 63``.

No assumptions are made about the form of payload data in user-defined payloads and no guarantees are given for user-defined payloads.

Payload Types
-------------

Every standardized payload has a payload type.

When sending a payload, the payload type is sent with it to describe it.

The enumeration below defines the current defined payload types:

.. code-block:: python

    enum MessageTypes
    {
        timeValueV1 @0;
        
        resolveReqV1 @1;
        resolveResV1 @2;
        
        helloWorldReqV1 @3;
        helloWorldResV1 @4;

        vectorReqV1 @5;
        vectorResV1 @6;

        reverseResolveReqV1 @7;
        reverseResolveResV1 @8;
        fluxLifecycleEventV1 @9;

        registerResolveReqV1 @10;
        registerResolveResV1 @11;

        unregisterResolveReqV1 @12;
        unregisterResolveResV1 @13;

        historyReqV1 @14;
        historyResV1 @15;
    }

Below, the payload types are defined:

Time-Value Pairs
----------------

For sending metric time-value pairs.

This payload type encodes such a time-value pair.

.. code-block:: python

    struct TimeValueV1
    {
        time @0 :UInt64;
        value @1 :Float64;
    


Resolve Requests and Responses
-------------------------------------

A req/res pair for resolving names to addresses as described `in its own section <addressing.html>`_.

To resolve a name (e.g. ``{node=4, metric=power}``) place the key, value pairs into ``attributes``

.. code-block:: python

   ResolveReqV1.attributes=[{key="node", value="4"}, {key="metric", value="power"}]

The naming service will respond to a ResolveReqV1 with a ResolveResV1, which contains in ``matchingInterfaces`` a list of all addresses that resolve the name.

This can be more than one address as names such as ``{node=*, metric=power}`` match every node power sensor.

.. code-block:: python

    struct ResolveReqV1
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

    struct ResolveResV1
    {
    matchingInterfaces @0 :List(Address);
    }

Reverse Resolve Requests and Responses
--------------------------------------

This req/res pair does the inverse of Resolve(Req/Res)V1: It translates an address back to the name it corresponds to.

.. code-block:: python

    struct ReverseResolveReqV1
    {
        address @0 :Address;
    }

    struct ReverseResolveResV1
    {
        name @0 :List(KV);
    }

Registering Name/Address Pairs for resolving
--------------------------------------------

By sending RegisterResolveReqV1's to the naming service (Address: ``00000000-0000-0000-0000-0000010ca105``),
this registers a new name-address pair for later lookups.

The response from the naming service indicates whether this operation was successful (``success``) and
if registering was unsuccessful, includes an error message (``errMsg``).

.. code-block:: python

    struct RegisterResolveReqV1
    {
        name @0 :List(KV);
        address @1 :Address;
    }

    struct RegisterResolveResV1
    {
        success @0: Bool;
        errMsg @1: Text;
    }

Unregistering Name/Address Pairs for resolving
----------------------------------------------

By sending UnregisterResolveReqV1's to the naming service (Address: ``00000000-0000-0000-0000-0000010ca105``), the address
given is unregistered and all name-address mapping with this address are deleted from the naming service.


The response from the naming service indicates whether this operation was successful (``success``) and
if registering was unsuccessful, includes an error message (``errMsg``).

Possible error cases include trying to unregister an address that is not actually registered.

.. code-block:: python
   
    struct UnregisterResolveReqV1
    {
        address @0 :Address;
    }

    struct UnregisterResolveResV1
    {
        success @0: Bool;
        errMsg @1: Text;
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

Requesting historic data from the time-series database
------------------------------------------------------

.. note::
   For a deeper explanation what these types _mean_, see the `Time-Series Database <time_series_db.html>`_ section.

Historic data from the time-series database can be requested with a HistoryReqV1.

This request contains:

- ``address`` The address of the metric to query data from. The database saves the metric data under the address that it was published on.
- ``startTime``, ``endTime`` Interval [startTime, endTime] to query data from.
- ``intervalMax`` TODO: explain, leave as 0 for now.
- ``type`` either ``HistoryType::aggregate`` or ``HistoryType::flexTimeline``. If aggregate is selected you get one aggregated value over [startTime, endTime], giving you
  statistical measures such as the minimum and maximum over the time interval. If flexTimeline is selected, you get individual time-value pairs over the interval [startTime, endTime].


The time-series database responds wit a HistoryResV1. This contains one union ``okOrErr`` that either contains

- ``ok`` a struct History with the historic data for that query.
- ``err`` a string with an error message, if the request could not be satisfied (e.g. the requested address does not exist)

.. note::
   Unlike C unions, Cap'n'Proto unions very clearly indicate which element is active, which can be queried.

The ``struct History`` contains the timestamps in ``timeDelta`` and a union ``rawOrAgg`` that contains either:

- ``value`` A list of floats, the raw  metric event data, as it was published originally.
- ``aggregate`` A list of ``struct Aggregate`` with pre-aggregated data over the requested time interval.

.. note::
   For further information about automatic pre-aggregation, see the `Time Series DB <time_series_db.html>`_ documentation.


The ``struct Aggregate`` contains statical measures of the metric in the given, aggregated interval, ``minimum``, ``maximum``, number of metric events in that interval (``sum``), etc.

If a single aggregate was requested ``HistoryReqV1.type == HistoryType::aggregate``. Then ``timeDelta`` will be empty and ``History.rawOrAgg.aggregate`` will contain exactly one element. (The requested aggregate).


If a timeline was requested (``HistoryReqV1.type == HistoryType::flexTimeline``), then ``length(timeDelta) == length(aggregate)`` or ``length(timeDelta) == length(value)``.
There need to be as many values as timestamps, obviously.

If the query returns raw values, ``rawOrAgg.value`` will contain the values, if the query returns pre-aggregated data, then ``rawOrAgg.aggregate`` contains the pre-aggregated elements over the interval.

.. code-block:: python

    struct Aggregate
    {
        minimum @0: Float64;
        maximum @1: Float64;
        sum @2: Float64;
        count @3: UInt64;
        integral @4: Float64;
        activeTime @5: UInt64;
    }

    struct History
    {
        timeDelta @0: List(Int64);
        rawOrAgg :union
        {
            value @1: List(Float64);
            aggregate @2: List(Aggregate);
        }
    }

    enum HistoryType 
    {
        aggregate @0;
        flexTimeline @1;
    }

    struct HistoryReqV1
    {
        address @0: Address;
        startTime @1: Int64;
        endTime @2: Int64;
        intervalMax @3: Int64;
        type @4: HistoryType;
    }

    struct HistoryResV1
    {
        okOrErr :union
        {
            ok @0 :History;
            err @1 :Text;
        }
    }
