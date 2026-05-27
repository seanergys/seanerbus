#![allow(async_fn_in_trait)]
// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use seanerbus_client::Connection;
use seanerbus_msg::message::Message;
use seanerbus_schemas::message::Address;
use uuid::Uuid;


pub trait ResolveExt {
    async fn resolve(&mut self, name: Vec<(String, String)>) -> Vec<Address>;
    async fn reverse_resolve(&mut self, addr: Address) -> Vec<(String, String)>;
}

impl ResolveExt for Connection {
    async fn resolve(&mut self, name: Vec<(String, String)>) -> Vec<Address> {
        let (payload_type, payload) = Message::ResolveReqV1(name).to_capnp().unwrap();
        let msg = self
            .request(
                Address(Uuid::from_u64_pair(0, 0x10ca105)),
                payload_type,
                &payload,
            )
            .await
            .unwrap();

        if let Message::ResolveResV1(addrs) =
            Message::from_capnp(msg.payload_type, &msg.payload).unwrap()
        {
            addrs.into_iter().map(Address).collect()
        } else {
            panic!("Got wrong message type as answer");
        }
    }
    async fn reverse_resolve(&mut self, addr: Address) -> Vec<(String, String)> {
        let (payload_type, payload) = Message::ReverseResolveReqV1(addr.0).to_capnp().unwrap();
        let msg = self
            .request(
                Address(Uuid::from_u64_pair(0, 0x10ca105)),
                payload_type,
                &payload,
            )
            .await
            .unwrap();

        if let Message::ReverseResolveResV1(name) =
            Message::from_capnp(msg.payload_type, &msg.payload).unwrap()
        {
            name
        } else {
            panic!("Got wrong message type as answer");
        }
    }
}
