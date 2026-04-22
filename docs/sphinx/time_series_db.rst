.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Time-Series Database
====================

.. note::
   There are ready-to-use `C++ <time_series_cpp.html>`_, `Python <time_series_python.html>`_ and Rust time-series query databases.

SeanerBUS includes a time-series database. This database is a SeanerBUS connected `MetricQ HTA <https://github.com/metricq/hta>`_.

In the context of  the Dataplane, the time-series database has two purposes:

- Saving metric events by subscribing to Addresses that publish :class:`seanerbus.msg.TimeValueV1` time-value pairs.
- Serving history metric data by responding to :class:`seanerbus.msg.HistoryReqV1` with :class:`seanerbus.msg.HistoryResV1`.


Building and Running the Database
---------------------------------

The database lives in `database/`. It is a C++ application and can thus be build with cmake:

.. code-block:: sh

   cd database/
   mkdir build
   cd build
   cmake ..
   make -j4

Running the database is very easy:

.. code-block:: sh

   ./db

For the history request/response interface, the database will chose a random address and then register that address as ``{"service" : "time_series_db"}`` to the `naming server <addressing.html>`_.

Configuration
-------------

The database is configured through a ``metrics.list`` file.


The time-series database will try to open ``metrics.list`` in the directory it is started in.

The ``metrics.list`` file contains a list of addresses each on its own line.

When the database service is started, it will subscribe to every address listed in ``metrics.list`` and store the received :class:`seanerbus.msg.TimeValueV1` in its database.

.. code-block:: sh

   # Example content of metrics.list

   6cc8ee26-d8ca-44af-8c74-08d96ac7418b
   a829a090-d415-4770-afe3-0afc3ac03598
