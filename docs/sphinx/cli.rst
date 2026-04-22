.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

SeanerBUS CLI tool
==================

``cli/`` contains a SeanerBUS CLI from which you can interact with the dataplane from the command line.

``cli subscribe``
-----------------

Subscribe to the given address and print the received payloads on standard output.

Example:

.. code-block:: sh

   uv run ./cli.py subscribe --addr d7578cb7-22ea-42ba-b746-dbd8eb43fb23

.. option:: -a <ADDR>, --addr <ADDR>

   Subscribe to this address.

``cli publish``
---------------

Publish a single event from the command line

Example:

.. code-block:: sh

    cli.py publish -t TimeValueV1 -a 4d55d469-d936-4d5e-a348-d03f38e938ca 'time=15' 'value=10'

.. option:: -t <type>, --type <type>

   Payload type of the message, e.g. TimeValueV1, FluxLifecycleEventV1, etc..

.. option:: -a <addr>, --addr <addr>

   Address to publish the event to

.. option:: key1=value1 [key2=value2] ...

   Sets the fields in the payload to the given values.

``cli resolve``
---------------

Resolves the name to the address(es)

Example:

.. code-block:: sh

   cli.py resolve 'node=4' 'sensor=power'


.. option:: key1=value1 [key2=value2] ...

   key=value pairs of the name

``cli reverse-resolve``
-----------------------

Resolves an address back to its name

Example:

.. code-block:: sh

   cli.py reverse-resolve d7578cb7-22ea-42ba-b746-dbd8eb43fb23

.. option:: <addr>

   Address to resolve back to its name

``cli register-resolve``
------------------------

Registers the address name pair in the naming service.

Example:

.. code-block:: sh

   cli.py register-resolve d7578cb7-22ea-42ba-b746-dbd8eb43fb23 'node=4' 'sensor=power'

.. option:: <addr>

   address to register the name to

.. option:: key1=value1 [key2=value2] ...

   key=value pairs of the name

``cli unregister-resolve``
-------------------------

Unregisters the address from the naming service

Example:

.. code-block:: sh

   cli.py unregister-resolve d7578cb7-22ea-42ba-b746-dbd8eb43fb23 

.. option:: <addr>

   Address to unregister

``cli history-aggregate``
-------------------------

Queries an aggregate of historic data from the time-series database.

This aggregate has statistical measures (minimum, maximum, mean, etc.) over the queried interval.

Example:

.. code-block:: sh

   term> uv run ./cli.py history-aggregate -s 10 -e 100 -i 10  4d55d469-d936-4d5e-a348-d03f38e938ca
   Minimum: 10.0
   Maximum: 10.0
   Sum: 30.0
   Count: 3
   Integral: 30.0
   Active Time: 3

.. option:: -s <start>, --start <start>

   Start of the time interval to query

.. option:: -e <end>, --end <end>

   End of the interval to query

.. option:: -i <interval-max>, --interval-max <interval-max>

   TODO: Document

.. option:: <addr>

   Address of the metric to query.

``cli history-timeline``
-------------------------

Queries an timeline of multiple historic metric values from the time-series database.

This aggregate has statistical measures (minimum, maximum, mean, etc.) over the queried interval.

Example:

.. code-block:: sh

   term> uv run ./cli.py history-timeline -s 10 -e 100 -i 10  4d55d469-d936-4d5e-a348-d03f38e938ca
    time, value
    13, 10.0
    14, 10.0
    15, 10.0
    16, 10.0

.. option:: -s <start>, --start <start>

   Start of the time interval to query

.. option:: -e <end>, --end <end>

   End of the interval to query

.. option:: -i <interval-max>, --interval-max <interval-max>

   TODO: Document

.. option:: <addr>

   Address of the metric to query.
