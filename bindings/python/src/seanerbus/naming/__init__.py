# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

from ..client import Connection
from ..msg import ReverseResolveReqV1, ReverseResolveResV1
from ..msg import ResolveResV1, ResolveReqV1
import uuid


async def resolve(name):
    """Use the naming service to translate a Name to an Address"""
    c = await Connection.connect("localhost", 5398)

    msg = ResolveResV1.from_capnp(
        await c.request(uuid.UUID(int=0x10CA105), ResolveReqV1(name).to_capnp())
    )

    return msg.uuids


async def reverse_resolve(addr):
    """Uses the naming service to translate a Address back to a Name"""
    c = await Connection.connect("localhost", 5398)

    msg = ReverseResolveResV1.from_capnp(
        await c.request(uuid.UUID(int=0x10CA105), ReverseResolveReqV1(addr).to_capnp())
    )

    return msg.name
