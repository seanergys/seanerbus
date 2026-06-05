.. SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
..
.. SPDX-License-Identifier: MIT

Python API
==========

The SeanerBUS has a Python API.

This API contains the following submodules

- For connecting to the SeanerBUS and publishing/subscribing/requesting or responding data, the `seanerbus.client <client_python.html>`_ module.
- For serializing/deserializing Message to send over the SeanerBUS, the `seanerbus.msg <msg_python.html>`_ module.
- For looking up names to addresses, addresses to names and registering new names for addresses, the `seanerbus.naming <naming_python.html>`_ module.
- For requesting historic metric data from the time-series database, the `seanerbus.time_series <time_series_python.html>`_ module.

.. toctree::
   client_python
   msg_python
   naming_python
   time_series_python
