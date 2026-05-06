#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

import seanerbus.client
from seanerbus.msg import TimeValueV1
import capnp
import asyncio
import uuid


async def main():
    c = await seanerbus.client.Connection.connect("localhost", 5398)

    await c.subscribe(uuid.UUID(int=10))

    tv = TimeValueV1.from_capnp(await c.read_msg())
    if tv.time != 0 or tv.value != 0.0:
        exit(1)
    tv = TimeValueV1.from_capnp(await c.read_msg())
    if tv.time != 1 or tv.value != 1.0:
        exit(1)
    tv = TimeValueV1.from_capnp(await c.read_msg())
    if tv.time != 2 or tv.value != 2.0:
        exit(1)
    exit(0)


asyncio.run(capnp.run(main()))
