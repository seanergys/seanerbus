#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

import argparse
import seanerbus_msg
import seanerbus_client
import asyncio
import capnp
import uuid


def print_msg(msg):
    print(msg[0], "==", msg[1])


def parse_msg(payload_type, payload):
    parsed_payload_type = None
    for var in vars(seanerbus_msg.msg_capnp.MessageTypes):
        if getattr(seanerbus_msg.msg_capnp.MessageTypes, var) == payload_type:
            parsed_payload_type = var
    payload_class = parsed_payload_type[:1].upper() + parsed_payload_type[1:]

    with getattr(seanerbus_msg.msg_capnp, payload_class).from_bytes(payload) as msg:
        return (payload_class, msg)


async def make_publish(args):
    address = uuid.UUID(args.address)
    payload_content = getattr(seanerbus_msg.msg_capnp, args.type).new_message()
    payload_type = getattr(
        seanerbus_msg.msg_capnp.MessageTypes, args.type[:1].lower() + args.type[1:]
    )

    for kv in args.keyvalue:
        kv = kv.split("=")
        if len(kv) != 2:
            print(
                f"{kv}: key=value arguments need to consist of two terms separated by a '=' sign!"
            )
            exit(1)
        try:
            kv[1] = float(kv[1])
        except ValueError:
            pass
        setattr(payload_content, kv[0], kv[1])

    c = await seanerbus_client.Connection.connect("localhost", 5398)
    await c.publish(address, (payload_type, payload_content.to_bytes()))


async def make_subscribe(args):
    address = uuid.UUID(args.address)
    c = await seanerbus_client.Connection.connect("localhost", 5398)
    await c.subscribe(address)

    while True:
        msg = await c.read_msg()
        print(msg.payloadType)
        print_msg(parse_msg(msg.payloadType, msg.payload))


parser = argparse.ArgumentParser(
    prog="SeanerBUS CLI interface",
    description="Command line tool for interacting with the SeanerBUS",
)
subparsers = parser.add_subparsers(help="foobar", dest="cmd")

subscribe_parser = subparsers.add_parser("subscribe", help="Subscribe to some address")
subscribe_parser.add_argument("-a", "--address", help="Address to subscribe to")

publish_parser = subparsers.add_parser("publish", help="Publish a message")
publish_parser.add_argument("-t", "--type", help="SeanerBUS payload type to publish")
publish_parser.add_argument("-a", "--address", help="UUID address to publish to")
publish_parser.add_argument(
    "keyvalue", nargs="*", help="key=value pairs to put into the payload"
)

args = parser.parse_args()

# print(vars(seanerbus_msg.msg_capnp.MessageTypes))
if args.cmd == "publish":
    asyncio.run(capnp.run(make_publish(args)))

if args.cmd == "subscribe":
    asyncio.run(capnp.run(make_subscribe(args)))
