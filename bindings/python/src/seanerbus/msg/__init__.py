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


class ResolveReqV1:
    def __init__(self, kvs):
        self.kvs = kvs

    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a NamingServiceReqV1V1"""
        res = ResolveReqV1({})

        if msg.payloadType != msg_capnp.MessageTypes.resolveReqV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as NamingServiceReqV1!"
            )

        with msg_capnp.ResolveReqV1.from_bytes(msg.payload) as req:
            for i in req.attributes:
                res.kvs[i.key] = i.value
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this NamingServiceReqV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.ResolveReqV1.new_message()

        list_of_kvs = list(self.kvs.items())

        msg.init("attributes", len(list_of_kvs))

        for num, element in enumerate(list_of_kvs):
            msg.attributes[num].key = element[0]
            msg.attributes[num].value = element[1]
        return (msg_capnp.MessageTypes.resolveReqV1, msg.to_bytes())


class ResolveResV1:
    def __init__(self, uuids):
        self.uuids = uuids

    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a ResolveResV1"""
        res = ResolveResV1([])

        if msg.payloadType != msg_capnp.MessageTypes.resolveResV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as ResolveResV1!"
            )

        with msg_capnp.ResolveResV1.from_bytes(msg.payload) as req:
            for i in req.matchingInterfaces:
                uuid_num = (i.upper << 64) + i.lower
                res.uuids.append(uuid.UUID(int=uuid_num))
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this ResolveResV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.ResolveResV1.new_message()

        msg.init("matchingInterfaces", len(self.uuids))

        for num, uuid_in in enumerate(self.uuids):
            (upper, lower) = uuid_to_u64_pair(uuid_in)
            msg.matchingInterfaces[num].upper = upper
            msg.matchingInterfaces[num].lower = lower
        return (msg_capnp.MessageTypes.resolveResV1, msg.to_bytes())


class ReverseResolveReqV1:
    def __init__(self, uuid):
        self.uuid = uuid

    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a ResolveResV1"""
        res = ReverseResolveReqV1([])

        if msg.payloadType != msg_capnp.MessageTypes.resolveResV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as ResolveResV1!"
            )

        with msg_capnp.ReverseResolveReqV1.from_bytes(msg.payload) as req:
            res.uuid = (req.address.upper << 64) + req.address.lower
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this ResolveResV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.ReverseResolveReqV1.new_message()

        (upper, lower) = uuid_to_u64_pair(self.uuid)
        msg.address.upper = upper
        msg.address.lower = lower
        return (msg_capnp.MessageTypes.reverseResolveReqV1, msg.to_bytes())


class ReverseResolveResV1:
    def __init__(self, name):
        self.name = name

    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a NamingServiceReqV1V1"""
        res = ReverseResolveResV1({})

        if msg.payloadType != msg_capnp.MessageTypes.reverseResolveResV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as NamingServiceReqV1!"
            )

        with msg_capnp.ReverseResolveResV1.from_bytes(msg.payload) as req:
            for i in req.name:
                res.name[i.key] = i.value
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this NamingServiceReqV1 as a Cap'n'Proto payload, so that it can be used with Connection.subscribe()"""
        msg = msg_capnp.ReverseResolveResV1.new_message()

        list_of_kvs = list(self.name.items())

        msg.init("name", len(list_of_kvs))

        for num, element in enumerate(list_of_kvs):
            msg.name[num].key = element[0]
            msg.name[num].value = element[1]
        return (msg_capnp.MessageTypes.reverseResolveResV1, msg.to_bytes())


class VectorReqV1:
    def __init__(self, values: list[float]):
        """Creates a new VectorReqV1 from the given list of floats"""
        self.values = values

    @staticmethod
    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg()) as a VectorReqV1"""
        req_obj = VectorReqV1([])

        if msg.payloadType != msg_capnp.MessageTypes.vectorReqV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as VectorReqV1!"
            )

        with msg_capnp.VectorReqV1.from_bytes(msg.payload) as req:
            # Convert Cap’n Proto list -> Python list
            req_obj.values = list(req.values)

        return req_obj

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this VectorReqV1 as a Cap'n Proto payload"""
        msg = msg_capnp.VectorReqV1.new_message()

        # Initialize list with proper size
        values_builder = msg.init("values", len(self.values))

        # Fill list
        for i, v in enumerate(self.values):
            values_builder[i] = v

        return (msg_capnp.MessageTypes.vectorReqV1, msg.to_bytes())


class VectorResV1:
    def __init__(self, results: list[float]):
        """Creates a new VectorResV1 from the given list of floats"""
        self.results = results

    @staticmethod
    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a VectorResV1"""
        res = VectorResV1([])

        if msg.payloadType != msg_capnp.MessageTypes.vectorResV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as VectorResV1!"
            )

        with msg_capnp.VectorResV1.from_bytes(msg.payload) as req:
            # Convert Cap’n Proto list → Python list
            res.results = list(req.results)

        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this VectorResV1 as a Cap'n Proto payload"""
        msg = msg_capnp.VectorResV1.new_message()

        results_builder = msg.init("results", len(self.results))

        # Fill values
        for i, v in enumerate(self.results):
            results_builder[i] = v

        return (msg_capnp.MessageTypes.vectorResV1, msg.to_bytes())


class FluxLifecycleEventV1:
    def __init__(self, data: str):
        """Creates a new FluxLifecycleEventV1 from the given list of floats"""
        self.data = data

    @staticmethod
    def from_capnp(msg):
        """Decodes the given message (as returned by Connection.read_msg() as a FluxLifecycleEventV1"""
        res = FluxLifecycleEventV1([])

        if msg.payloadType != msg_capnp.MessageTypes.fluxLifecycleEventV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as FluxLifecycleEventV1!"
            )

        with msg_capnp.FluxLifecycleEventV1.from_bytes(msg.payload) as req:
            # Convert Cap’n Proto list → Python list
            res.data = list(req.data)

        return res

    def to_capnp(self) -> tuple[int, bytes]:
        """Encodes this FluxLifecycleEventV1 as a Cap'n Proto payload"""
        msg = msg_capnp.FluxLifecycleEventV1.new_message()

        msg.data = self.data

        return (msg_capnp.MessageTypes.fluxLifecycleEventV1, msg.to_bytes())
