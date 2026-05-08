#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

import seanerbus.client
from seanerbus.msg import VectorReqV1, VectorResV1
import capnp
import asyncio
import uuid
import sys


async def main():
    # Connect to the SeanerBUS
    c = await seanerbus.client.Connection.connect("localhost", 5398)
    print(c)

    # Send a HelloWorldReqV1 to the UUID given on the command line.
    # Interpret the result as a HelloWorldResV1
    u = uuid.UUID(sys.argv[1])
    v = VectorReqV1([1.0, 2.0, 3.0]).to_capnp()
    a = await c.request(u, v)

    print("Received payloadType:", a.payloadType)

    msg = VectorResV1.from_capnp(a)

    print(f"Got a Response: {msg.results}")


if len(sys.argv) != 2:
    print("request_test.py UUID")
    exit(-1)

asyncio.run(capnp.run(main()))
