# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

import capnp
import os
import uuid

capnp.remove_import_hook()
msg_capnp = capnp.load(
    os.path.dirname(os.path.abspath(__file__))
    + "/../../../../internal/internal_schemas/schemas/internal.capnp"
)


def uuid_to_u64_pair(uuid: uuid.UUID) -> tuple[int, int]:
    upper = uuid.int >> 64
    lower = uuid.int & (1 << 64) - 1
    return (upper, lower)


class Connection:
    def __init__(self):
        pass

    @staticmethod
    async def connect(host: str, port: int):
        conn: Connection = Connection()

        conn.stream: capnp.AsyncIoStream = await capnp.AsyncIoStream.create_connection(
            host=host, port=port
        )
        return conn

    async def subscribe(self, subscribe_to: uuid.UUID):
        msg = msg_capnp.Message.new_message()
        (upper, lower) = uuid_to_u64_pair(subscribe_to)
        msg.address.upper = upper
        msg.address.lower = lower
        msg.type = "subscribe"
        msg.payloadType = 0
        msg.payload = b""
        await msg.write_async(self.stream)

    async def register(self, register_to: uuid.UUID):
        msg = msg_capnp.Message.new_message()
        (upper, lower) = uuid_to_u64_pair(register_to)
        msg.address.upper = upper
        msg.address.lower = lower
        msg.type = "register"
        msg.payloadType = 0
        msg.payload = b""
        await msg.write_async(self.stream)

    async def publish(self, addr: uuid.UUID, payload: tuple[int, bytes]):
        msg = msg_capnp.Message.new_message()
        (upper, lower) = uuid_to_u64_pair(addr)
        msg.address.upper = upper
        msg.address.lower = lower
        msg.type = "publish"
        msg.payloadType = payload[0]
        msg.payload = payload[1]
        await msg.write_async(self.stream)

    async def request(self, addr: uuid.UUID, payload: tuple[int, bytes]):
        msg = msg_capnp.Message.new_message()
        (upper, lower) = uuid_to_u64_pair(addr)
        msg.address.upper = upper
        msg.address.lower = lower
        msg.type = "request"
        msg.payloadType = payload[0]
        msg.payload = payload[1]
        await msg.write_async(self.stream)
        message = await msg_capnp.Message.read_async(self.stream)
        return message

    async def respond_to(self, from_msg, payload: tuple[int, bytes]):
        msg = msg_capnp.Message.new_message()
        msg.address.upper = from_msg.address.upper
        msg.address.lower = from_msg.address.lower
        msg.type = "response"
        msg.to.upper = getattr(from_msg, "from").upper
        msg.to.lower = getattr(from_msg, "from").lower
        msg.payloadType = payload[0]
        msg.payload = payload[1]
        await msg.write_async(self.stream)

    async def read_msg(self):
        message = await msg_capnp.Message.read_async(self.stream)
        return message
