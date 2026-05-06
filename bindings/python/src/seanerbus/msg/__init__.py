# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

import capnp
import os
import uuid


def uuid_to_u64_pair(uuid: uuid.UUID) -> tuple[int, int]:
    upper = uuid.int >> 64
    lower = uuid.int & (1 << 64) - 1
    return (upper, lower)


msg_capnp = capnp.load(os.path.dirname(os.path.abspath(__file__)) + "/msg.capnp")


class TimeValueV1:
    def __init__(self, time, value):
        """Creates a TimeValueV1 from a timestamp and value"""
        self.time = time
        self.value = value

    @staticmethod
    def from_capnp(msg):
        """Takes a message (as returned by Connection.read_msg() and decode it as TimeValueV1 returning it."""
        res = TimeValueV1(0, 0.0)
        if msg.payloadType != msg_capnp.MessageTypes.timeValueV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as TimeValueV1 message!"
            )
        with msg_capnp.TimeValueV1.from_bytes(msg.payload) as tv:
            res.time = tv.time
            res.value = tv.value
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this TimeValueV1 as a Cap'n'Proto Payload"""
        msg = msg_capnp.TimeValueV1.new_message()
        msg.time = self.time
        msg.value = self.value
        return (msg_capnp.MessageTypes.timeValueV1, msg.to_bytes())


class HelloWorldReqV1:
    def __init__(self, msg: str):
        """Creates a new HelloWorldReqV1 from the given string"""
        self.msg = msg

    @staticmethod
    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a HelloWorldReqV1"""
        res = HelloWorldReqV1("")

        if msg.payloadType != msg_capnp.MessageTypes.helloWorldReqV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as HelloWorldReqV1!"
            )

        with msg_capnp.HelloWorldReqV1.from_bytes(msg.payload) as req:
            res.msg = req.msg
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this HelloWorldReqV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.HelloWorldReqV1.new_message()
        msg.msg = self.msg
        return (msg_capnp.MessageTypes.helloWorldReqV1, msg.to_bytes())


class HelloWorldResV1:
    def __init__(self, msg: str):
        """Creates a new HelloWorldResV1 from the given string"""
        self.msg = msg

    @staticmethod
    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a HelloWorldResV1"""
        res = HelloWorldResV1("")

        if msg.payloadType != msg_capnp.MessageTypes.helloWorldResV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as HelloWorldResV1!"
            )

        with msg_capnp.HelloWorldResV1.from_bytes(msg.payload) as req:
            res.msg = req.msg
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this HelloWorldResV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.HelloWorldResV1.new_message()
        msg.msg = self.msg
        return (msg_capnp.MessageTypes.helloWorldResV1, msg.to_bytes())


class NamingServiceReqV1:
    def __init__(self, kvs):
        self.kvs = kvs

    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a NamingServiceReqV1V1"""
        res = NamingServiceReqV1({})

        if msg.payloadType != msg_capnp.MessageTypes.namingServiceReqV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as NamingServiceReqV1!"
            )

        with msg_capnp.NamingServiceReqV1.from_bytes(msg.payload) as req:
            for i in req.attributes:
                print(i)
                res.kvs[i.key] = i.value
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this NamingServiceReqV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.NamingServiceReqV1.new_message()

        list_of_kvs = list(self.kvs.items())

        msg.init("attributes", len(list_of_kvs))

        for num, element in enumerate(list_of_kvs):
            print(element)
            msg.attributes[num].key = element[0]
            msg.attributes[num].value = element[1]
        return (msg_capnp.MessageTypes.namingServiceReqV1, msg.to_bytes())


class NamingServiceResV1:
    def __init__(self, uuids):
        self.uuids = uuids

    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a NamingServiceResV1"""
        res = NamingServiceResV1([])

        if msg.payloadType != msg_capnp.MessageTypes.namingServiceResV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as NamingServiceResV1!"
            )

        with msg_capnp.NamingServiceResV1.from_bytes(msg.payload) as req:
            for i in req.matchingInterfaces:
                uuid_num = (i.upper << 64) + i.lower
                res.uuids.append(uuid.UUID(int=uuid_num))
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this NamingServiceResV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.NamingServiceResV1.new_message()

        msg.init("matchingInterfaces", len(self.uuids))

        for num, uuid_in in enumerate(self.uuids):
            (upper, lower) = uuid_to_u64_pair(uuid_in)
            msg.matchingInterfaces[num].upper = upper
            msg.matchingInterfaces[num].lower = lower
        return (msg_capnp.MessageTypes.namingServiceResV1, msg.to_bytes())
