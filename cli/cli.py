#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

import argparse
import seanerbus.msg
import seanerbus.client
import seanerbus.naming
import asyncio
import capnp
import uuid


def print_msg(msg):
    print(msg[0], "==", msg[1])


def parse_msg(payload_type, payload):
    parsed_payload_type = None
    for var in vars(seanerbus.msg.msg_capnp.MessageTypes):
        if getattr(seanerbus.msg.msg_capnp.MessageTypes, var) == payload_type:
            parsed_payload_type = var
    payload_class = parsed_payload_type[:1].upper() + parsed_payload_type[1:]

    with getattr(seanerbus.msg.msg_capnp, payload_class).from_bytes(payload) as msg:
        return (payload_class, msg)


async def make_publish(args):
    address = uuid.UUID(args.address)
    payload_content = getattr(seanerbus.msg.msg_capnp, args.type).new_message()
    payload_type = getattr(
        seanerbus.msg.msg_capnp.MessageTypes, args.type[:1].lower() + args.type[1:]
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

    c = await seanerbus.client.Connection.connect("localhost", 5398)
    await c.publish(address, (payload_type, payload_content.to_bytes()))


async def make_subscribe(args):
    address = uuid.UUID(args.address)
    c = await seanerbus.client.Connection.connect("localhost", 5398)
    await c.subscribe(address)

    while True:
        msg = await c.read_msg()
        print(msg.payloadType)
        print_msg(parse_msg(msg.payloadType, msg.payload))


async def make_resolve(args):
    resolv = {}
    for kv in args.keyvalue:
        kv = kv.split("=")
        if len(kv) != 2:
            print(
                f"{kv}: key=value arguments need to consist of two terms separated by a '=' sign!"
            )
            exit(1)
        resolv[kv[0]] = kv[1]
    addrs = await seanerbus.naming.resolve(resolv)
    print(f"Resolved {resolv} to:")
    for addr in addrs:
        print(f"\t{addr}")


async def make_rev_resolve(args):
    addr = uuid.UUID(args.uuid)

    kvs = await seanerbus.naming.reverse_resolve(addr)
    print(f"'{addr} => {kvs}")


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

resolve_parser = subparsers.add_parser("resolve", help="Resolve a name")
resolve_parser.add_argument("keyvalue", nargs="*", help="key=value pairs to resolve")

rev_resolve_parser = subparsers.add_parser(
    "reverse-resolve", help="Reverse resolve address to name"
)
rev_resolve_parser.add_argument("uuid", help="Address to resolve back")
args = parser.parse_args()

# print(vars(seanerbus.msg.msg_capnp.MessageTypes))
if args.cmd == "publish":
    asyncio.run(capnp.run(make_publish(args)))

if args.cmd == "subscribe":
    asyncio.run(capnp.run(make_subscribe(args)))

if args.cmd == "resolve":
    asyncio.run(capnp.run(make_resolve(args)))

if args.cmd == "reverse-resolve":
    asyncio.run(capnp.run(make_rev_resolve(args)))
