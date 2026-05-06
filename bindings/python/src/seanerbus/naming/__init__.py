# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

from ..client import Connection
from ..msg import NamingServiceResV1, NamingServiceReqV1
import uuid


async def resolve(kvs):
    c = await Connection.connect("localhost", 5398)

    msg = NamingServiceResV1.from_capnp(
        await c.request(uuid.UUID(int=0x10CA105), NamingServiceReqV1(kvs).to_capnp())
    )

    return msg.uuids
