import capnp
import os

msg_capnp = capnp.load(
    os.path.dirname(os.path.abspath(__file__)) + "/../../../schemas/msg.capnp"
)


class TimeValueV1:
    def __init__(self, time, value):
        self.time = time
        self.value = value

    @staticmethod
    def from_capnp(msg):
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
        msg = msg_capnp.TimeValueV1.new_message()
        msg.time = self.time
        msg.value = self.value
        return (msg_capnp.MessageTypes.timeValueV1, msg.to_bytes())


class HelloWorldReqV1:
    def __init__(self, msg: str):
        self.msg = msg

    @staticmethod
    def from_capnp(msg):
        res = HelloWorldReqV1("")

        if msg.payloadType != msg_capnp.MessageTypes.helloWorldReqV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as HelloWorldReqV1!"
            )

        with msg_capnp.HelloWorldReqV1.from_bytes(msg.payload) as req:
            res.msg = req.msg
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        msg = msg_capnp.HelloWorldReqV1.new_message()
        msg.msg = self.msg
        return (msg_capnp.MessageTypes.helloWorldReqV1, msg.to_bytes())


class HelloWorldResV1:
    def __init__(self, msg: str):
        self.msg = msg

    @staticmethod
    def from_capnp(msg):
        res = HelloWorldResV1("")

        if msg.payloadType != msg_capnp.MessageTypes.helloWorldResV1:
            raise RuntimeError(
                f"Can not interpret message of type {msg.payloadType} as HelloWorldResV1!"
            )

        with msg_capnp.HelloWorldResV1.from_bytes(msg.payload) as req:
            res.msg = req.msg
        return res

    def to_capnp(self) -> tuple[int, bytes]:
        msg = msg_capnp.HelloWorldResV1.new_message()
        msg.msg = self.msg
        return (msg_capnp.MessageTypes.helloWorldResV1, msg.to_bytes())
