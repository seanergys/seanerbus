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

    # Registers as a request-responder with the broker
    await c.register(uuid.UUID(sys.argv[1]))

    while True:
        # Wait for a message
        msg = await c.read_msg()
        # Try to decode its payload as a HelloWorldReqV1
        payload = VectorReqV1.from_capnp(msg)

        print(f"Got a VectorReqV1: {payload}")

        # Respond to it with a HelloWorldResV1
        await c.respond_to(msg, VectorResV1(payload.values).to_capnp())


if len(sys.argv) != 2:
    print("subscribe_test.py UUID")
    exit(-1)

asyncio.run(capnp.run(main()))
