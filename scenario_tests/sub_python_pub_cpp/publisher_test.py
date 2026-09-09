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
    c = seanerbus.client.Connection("demobus://localhost:5398", "demobus://localhost:5398")

    handle = await c.subscribe(str(uuid.UUID(int=10)))

    tv = TimeValueV1.from_capnp(await handle())
    if tv.time != 1 or tv.value != 1.0:
        exit(1)
    tv = TimeValueV1.from_capnp(await handle())
    if tv.time != 2 or tv.value != 2.0:
        exit(1)
    tv = TimeValueV1.from_capnp(await handle())
    if tv.time != 3 or tv.value != 3.0:
        exit(1)
    exit(0)


asyncio.run(capnp.run(main()))
